#include "memory.h"
#include "common/common.h"
#include "virt_mem_manager/virt_mem_manager.h"
#include "page_manager/phys_mem_manager.h"
#include "kmalloc/kmalloc.h"
#include "smp/smp.h"
#include "synchronization.h"
#include "debug_gfx.h"
#include "interrupts.h"
#include "managers.h"
#include "target/x86_64/apic/apic.h"
#include "libs/libCardinal/include/shared_memory.h"

static Spinlock vmem_lock = NULL;
static ManagedPageTable volatile **curPageTable = NULL;

void
HandleTLBShootdown(uint32_t UNUSED(int_no),
                   uint32_t UNUSED(err_code));

void
MemoryHAL_Initialize(void) {
    if(vmem_lock == NULL)
        vmem_lock = CreateBootstrapSpinlock();

    RegisterInterruptHandler(0xE, VirtMemMan_HandlePageFault);
    RegisterInterruptHandler(34, HandleTLBShootdown);

    if(curPageTable == NULL)
        curPageTable = (ManagedPageTable volatile **)AllocateAPLSMemory(sizeof(ManagedPageTable**));
}

void*
GetVirtualAddress(CachingMode c,
                  void *physicalAddress) {
    void* ret = VirtMemMan_GetVirtualAddress(c, physicalAddress);
    return ret;
}

void*
GetPhysicalAddress(void *virtualAddress) {
    void *ret = GetPhysicalAddressPageTable(GetActiveVirtualMemoryInstance(),
                                            virtualAddress);
    return ret;
}

void*
GetPhysicalAddressPageTable(ManagedPageTable 	*src,
                            void 	*virtualAddress) {
    LockSpinlock(src->lock);
    void *ret = VirtMemMan_GetPhysicalAddress((PML_Instance)src->PageTable, virtualAddress);
    UnlockSpinlock(src->lock);
    return ret;
}

MemoryAllocationErrors
CreateVirtualMemoryInstance(ManagedPageTable *inst) {
    if(inst != NULL) {
        inst->PageTable = (UID)VirtMemMan_CreateInstance();
        return MemoryAllocationErrors_None;
    }
    return MemoryAllocationErrors_Unknown;
}

void
FreeVirtualMemoryInstance(ManagedPageTable *inst) {
    if(inst != NULL) {
        if(inst->reference_count != 0)
            HaltProcessor();

        LockSpinlock(vmem_lock);
        VirtMemMan_FreePageTable((PML_Instance)inst->PageTable);
        UnlockSpinlock(vmem_lock);
    }
}

ManagedPageTable*
SetActiveVirtualMemoryInstance(ManagedPageTable *inst) {
    ManagedPageTable *tmp = (ManagedPageTable*)*curPageTable;
    *curPageTable = inst;
    LockSpinlock(vmem_lock);
    LockSpinlock(inst->lock);

    inst->reference_count++;
    VirtMemMan_SetCurrent((PML_Instance)inst->PageTable);
    UnlockSpinlock(inst->lock);

    if(tmp != NULL) {
        AtomicDecrement32(&tmp->reference_count);
    }

    UnlockSpinlock(vmem_lock);
    return (ManagedPageTable*)tmp;
}

ManagedPageTable*
GetActiveVirtualMemoryInstance(void) {
    VirtMemMan_GetCurrent();
    return (ManagedPageTable*)*curPageTable;
}

MemoryAllocationErrors
MapPage(ManagedPageTable *pageTable,
        uint64_t 		physicalAddress,
        uint64_t 		virtualAddress,
        size_t			size,
        CachingMode 		cacheMode,
        MemoryAllocationType 	allocType,
        MemoryAllocationFlags 	flags) {

    if(virtualAddress == 0)return MemoryAllocationErrors_Unknown;

    if(virtualAddress % 4096)__asm__ ("cli\n\thlt" :: "a"(virtualAddress));


    MEM_TYPES cache = 0;
    if(cacheMode == CachingModeUncachable)cache = MEM_TYPE_UC;
    else if(cacheMode == CachingModeWriteBack)cache = MEM_TYPE_WB;
    else if(cacheMode == CachingModeWriteThrough)cache = MEM_TYPE_WT;
    else return MemoryAllocationErrors_InvalidFlags;

    MEM_ACCESS_PERMS access = 0;
    if(flags & MemoryAllocationFlags_Exec)access |= MEM_EXEC;
    if(flags & MemoryAllocationFlags_Write)access |= MEM_WRITE;

    MEM_SECURITY_PERMS perms = 0;
    if((flags & MemoryAllocationFlags_Kernel) == MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if((flags & MemoryAllocationFlags_User) == MemoryAllocationFlags_User)perms |= MEM_USER;

    MemoryAllocationsMap *allocMap = NULL;
    if(((virtualAddress >> 39) & 0x1FF) != 511) {
        allocMap = kmalloc(sizeof(MemoryAllocationsMap));

        allocMap->CacheMode = cacheMode;
        allocMap->VirtualAddress = virtualAddress;
        allocMap->PhysicalAddress = physicalAddress;
        allocMap->Length = size;
        allocMap->Flags = flags;
        allocMap->AllocationType = allocType;
    }

    LockSpinlock(pageTable->lock);

    if(pageTable->AllocationMap != NULL) {
        MemoryAllocationsMap *map = pageTable->AllocationMap;
        MemoryAllocationsMap *prev = NULL;
        do {

            if(map->VirtualAddress <= virtualAddress && (map->VirtualAddress + map->Length) >= (virtualAddress + size)) {
                //TODO define the possible situations and update the allocation map appropriately
                MemoryAllocationsMap *top = kmalloc(sizeof(MemoryAllocationsMap));

                top->CacheMode = map->CacheMode;
                top->Flags = map->Flags;
                top->AllocationType = map->AllocationType;

                top->VirtualAddress = virtualAddress + size;
                top->PhysicalAddress = (uint64_t)VirtMemMan_GetPhysicalAddress((PML_Instance)pageTable->PageTable, (void*)(virtualAddress + size));
                top->Length = (map->VirtualAddress + map->Length) - (virtualAddress + size);

                map->Length = virtualAddress - map->VirtualAddress;

                if(top->Length != 0) {
                    top->next = map->next;
                    top->prev = map;
                    if(map->next != NULL)map->next->prev = top;
                    map->next = top;
                } else kfree(top);

                if(map->Length == 0) {
                    if(pageTable->AllocationMap != map) {
                        prev->next = map->next;
                        if(map->next != NULL)map->next->prev = prev;
                    } else {
                        pageTable->AllocationMap = map->next;
                        if(map->next != NULL)map->next->prev = NULL;
                    }

                    kfree(map);
                    break;
                }
            }

            prev = map;
            map = map->next;
        } while(map != NULL);
    }

    //At this point, all parameters have been verified
    if(((virtualAddress >> 39) & 0x1FF) != 511) {
        allocMap->next = pageTable->AllocationMap;
        if(pageTable->AllocationMap != NULL)pageTable->AllocationMap->prev = allocMap;
        allocMap->prev = NULL;
        pageTable->AllocationMap = allocMap;
    }

    if(allocType & MemoryAllocationType_Fork) {
        access = access & ~MEM_WRITE; //Forked pages are copy on write
    }

    VirtMemMan_Map((PML_Instance)pageTable->PageTable,
                   virtualAddress,
                   physicalAddress,
                   size,
                   TRUE,
                   cache,
                   access,
                   perms);

    UnlockSpinlock(pageTable->lock);

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
ChangePageFlags(ManagedPageTable *pageTable,
                uint64_t        virtualAddress,
                CachingMode         cacheMode,
                MemoryAllocationType    allocType,
                MemoryAllocationFlags   flags) {

    if(virtualAddress == 0)return MemoryAllocationErrors_Unknown;

    MEM_TYPES cache = 0;
    if(cacheMode == CachingModeUncachable)cache = MEM_TYPE_UC;
    else if(cacheMode == CachingModeWriteBack)cache = MEM_TYPE_WB;
    else if(cacheMode == CachingModeWriteThrough)cache = MEM_TYPE_WT;
    else return MemoryAllocationErrors_InvalidFlags;

    MEM_ACCESS_PERMS access = 0;
    if(flags & MemoryAllocationFlags_Exec)access |= MEM_EXEC;
    if(flags & MemoryAllocationFlags_Write)access |= MEM_WRITE;

    MEM_SECURITY_PERMS perms = 0;
    if((flags & MemoryAllocationFlags_Kernel) == MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if((flags & MemoryAllocationFlags_User) == MemoryAllocationFlags_User)perms |= MEM_USER;

    LockSpinlock(pageTable->lock);

    MemoryAllocationsMap *map = pageTable->AllocationMap;

    while(map != NULL) {
        if(map->VirtualAddress == virtualAddress)
            break;

        map = map->next;
    }

    if(map == NULL)
        return UnlockSpinlock(pageTable->lock), MemoryAllocationErrors_InvalidFlags;

    map->CacheMode = cacheMode;
    map->Flags = flags;
    map->AllocationType = allocType;

    //At this point, all parameters have been verified
    if(allocType & MemoryAllocationType_Fork) {
        access = access & ~MEM_WRITE; //Forked pages are copy on write
    }

    VirtMemMan_Map((PML_Instance)pageTable->PageTable,
                   virtualAddress,
                   map->PhysicalAddress,
                   map->Length,
                   TRUE,
                   cache,
                   access,
                   perms);

    UnlockSpinlock(pageTable->lock);

    return MemoryAllocationErrors_None;
}


MemoryAllocationErrors
UnmapPage(ManagedPageTable 	*pageTable,
          uint64_t 		     virtualAddress,
          size_t 		     size) {

    LockSpinlock(pageTable->lock);

    if(pageTable->AllocationMap != NULL) {
        MemoryAllocationsMap *map = pageTable->AllocationMap;
        MemoryAllocationsMap *prev = NULL;
        do {

            if(map->VirtualAddress <= virtualAddress && (map->VirtualAddress + map->Length) >= (virtualAddress + size)) {
                //TODO define the possible situations and update the allocation map appropriately
                MemoryAllocationsMap *top = kmalloc(sizeof(MemoryAllocationsMap));

                top->CacheMode = map->CacheMode;
                top->Flags = map->Flags;
                top->AllocationType = map->AllocationType;

                top->VirtualAddress = virtualAddress + size;
                top->PhysicalAddress = (uint64_t)VirtMemMan_GetPhysicalAddress((PML_Instance)pageTable->PageTable, (void*)(virtualAddress + size));
                top->Length = (map->VirtualAddress + map->Length) - (virtualAddress + size);

                map->Length = virtualAddress - map->VirtualAddress;

                if(top->Length != 0) {
                    top->next = map->next;
                    top->prev = map;
                    if(map->next != NULL)map->next->prev = top;
                    map->next = top;
                } else kfree(top);

                if(map->Length == 0) {
                    if(pageTable->AllocationMap != map) {
                        prev->next = map->next;
                        if(map->next != NULL)map->next->prev = prev;
                    } else {
                        pageTable->AllocationMap = map->next;
                        if(map->next != NULL)map->next->prev = NULL;
                    }

                    kfree(map);
                    break;
                }
            }

            prev = map;
            map = map->next;
        } while(map != NULL);
    }

    VirtMemMan_Unmap((PML_Instance)pageTable->PageTable,
                     virtualAddress,
                     (uint64_t)size);

    UnlockSpinlock(pageTable->lock);
    return MemoryAllocationErrors_None;
}


uint64_t
GetMemoryAllocationTypeTop(MemoryAllocationType allocType,
                           MemoryAllocationFlags sec_perms) {

    MEM_SECURITY_PERMS perms = 0;
    if(sec_perms & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if(sec_perms & MemoryAllocationFlags_User)perms |= MEM_USER;

    return VirtMemMan_GetAllocTypeTop(allocType, perms);
}

uint64_t
GetMemoryAllocationTypeBase(MemoryAllocationType allocType,
                            MemoryAllocationFlags sec_perms) {

    MEM_SECURITY_PERMS perms = 0;
    if(sec_perms & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if(sec_perms & MemoryAllocationFlags_User)perms |= MEM_USER;

    return VirtMemMan_GetAllocTypeBase(allocType, perms);
}

MemoryAllocationErrors
FindFreeVirtualAddress(ManagedPageTable *pageTable,
                       uint64_t			*virtualAddress,
                       size_t 			size,
                       MemoryAllocationType 	allocType,
                       MemoryAllocationFlags 	flags) {

    if(virtualAddress == NULL)return MemoryAllocationErrors_Unknown;

    LockSpinlock(pageTable->lock);

    MEM_SECURITY_PERMS perms = 0;
    if(flags & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if(flags & MemoryAllocationFlags_User)perms |= MEM_USER;

    void* addr = VirtMemMan_FindFreeAddress((PML_Instance)pageTable->PageTable,
                                            size,
                                            allocType,
                                            perms);

    if(addr != NULL)*virtualAddress = (uint64_t)addr;

    UnlockSpinlock(pageTable->lock);

    return MemoryAllocationErrors_None;
}

uint64_t
AllocatePhysicalPage(void) {
    LockSpinlock(vmem_lock);
    uint64_t ret = MemMan_Alloc();
    UnlockSpinlock(vmem_lock);
    return ret;
}

void
FreePhysicalPage(uint64_t ptr) {
    LockSpinlock(vmem_lock);
    MemMan_Free(ptr);
    UnlockSpinlock(vmem_lock);
}

uint64_t
AllocatePhysicalPageCont(int pageCount) {
    LockSpinlock(vmem_lock);
    uint64_t ret = MemMan_Alloc4KiBPageCont(pageCount);
    UnlockSpinlock(vmem_lock);
    return ret;
}

void
FreePhysicalPageCont(uint64_t ptr,
                     int pageCount) {
    LockSpinlock(vmem_lock);
    MemMan_FreeCont(ptr, pageCount);
    UnlockSpinlock(vmem_lock);
}

void*
AllocateAPLSMemory(uint64_t size) {
    LockSpinlock(vmem_lock);
    void* ret = VirtMemMan_AllocCoreLocalData(size);
    UnlockSpinlock(vmem_lock);
    return ret;
}

int
GetCoreCount(void) {
    return SMP_GetCoreCount();
}

static bool tlb_shootdown = FALSE;
static volatile int tlb_core_count = 0;

void
HandlePageFault(uint64_t virtualAddress,
                uint64_t instruction_pointer,
                MemoryAllocationFlags error) {

    if(!ProcessSys_IsInitialized()) {
        __asm__("cli\n\thlt" :: "a"(instruction_pointer));
    }
    //Check the current process's memory info table
    ProcessInformation *procInfo = NULL;
    GetProcessReference(GetCurrentProcessUID(), &procInfo);
    if(procInfo == NULL | procInfo->PageTable->AllocationMap == NULL) {
        __asm__("cli\n\thlt" :: "a"(instruction_pointer), "b"(1), "c"(procInfo->PageTable->AllocationMap));
        //while(1)debug_gfx_writeLine("Error: Page Fault");
    }

    LockSpinlock(procInfo->lock);
    MemoryAllocationsMap *map = procInfo->PageTable->AllocationMap;
    while(map != NULL) {
        if(virtualAddress >= map->VirtualAddress && virtualAddress < (map->VirtualAddress + map->Length)) {

            //Found an entry that describes this fault
            if(map->AllocationType & MemoryAllocationType_Application) {
                __asm__("cli\n\thlt" :: "a"(instruction_pointer), "b"(3), "c"(error));
            }


            break;
        }

        MemoryAllocationsMap *tmp_next = map->next;

        map = tmp_next;
    }

    if(map == NULL) {
        __asm__("cli\n\thlt" :: "a"(instruction_pointer));
    }
    UnlockSpinlock(procInfo->lock);
}

void
CheckAddressPermissions(ManagedPageTable      *pageTable,
                        uint64_t              addr,
                        CachingMode           *cacheMode,
                        MemoryAllocationFlags *flags) {
    MEM_TYPES cache = 0;
    MEM_ACCESS_PERMS access_perm = 0;
    MEM_SECURITY_PERMS sec_perm = 0;

    CachingMode c = 0;
    MemoryAllocationFlags a = 0;

    LockSpinlock(pageTable->lock);
    VirtMemMan_CheckAddressPermissions((PML_Instance)pageTable->PageTable, addr, &cache, &access_perm, &sec_perm);
    UnlockSpinlock(pageTable->lock);

    if(cache == 0 && access_perm == 0 && sec_perm == 0) {
        if(cacheMode != NULL)*cacheMode = 0;
        if(flags != NULL)*flags = 0;

        return;
    }

    switch(cache) {
    case MEM_TYPE_WT:
        c = CachingModeWriteThrough;
        break;
    case MEM_TYPE_WB:
        c = CachingModeWriteBack;
        break;
    case MEM_TYPE_UC:
        c = CachingModeUncachable;
        break;
    default:
        c = 0;
        break;
    }

    if(sec_perm & MEM_USER)a |= MemoryAllocationFlags_User;
    else a |= MemoryAllocationFlags_Kernel;

    if(access_perm & MEM_READ)a |= MemoryAllocationFlags_Read;
    if(access_perm & MEM_WRITE)a |= MemoryAllocationFlags_Write;
    if(access_perm & MEM_EXEC)a |= MemoryAllocationFlags_Exec;
    else a |= MemoryAllocationFlags_NoExec;

    if(cacheMode != NULL)*cacheMode = c;
    if(flags != NULL)*flags = a;
}

void
HaltProcessor(void) {
    __asm__ volatile("cli\n\thlt");
}

void
HandleTLBShootdown(uint32_t UNUSED(int_no),
                   uint32_t UNUSED(err_code)) {
    VirtMemMan_SetCurrent((PML_Instance)((*curPageTable)->PageTable));
    AtomicIncrement32((uint32_t*)&tlb_core_count);
}

void
PerformTLBShootdown(void) {
    //Reload the current virtual memory instance
    tlb_shootdown = TRUE;
    __asm__ volatile("mfence");
    APIC_SendIPI(0, APIC_DESTINATION_SHORT_ALLBUTSELF, 34, APIC_DELIVERY_MODE_FIXED);

    tlb_core_count = 1;
    while(tlb_core_count < GetCoreCount());
    tlb_core_count = 1;
    tlb_shootdown = FALSE;
}

uint64_t
SetupTemporaryWriteMap(ManagedPageTable *pageTable,
                       uint64_t addr,
                       size_t size) {
    //align the physical address to a page boundary and map it to the current page table
    if(size == 0)return 0;

    LockSpinlock(vmem_lock);

    if(size % PAGE_SIZE)size += PAGE_SIZE - size % PAGE_SIZE;

    addr -= addr % PAGE_SIZE;

    uint64_t tmp_loc_virt = 0;

    //First allocate the same amount of memory in another location
    FindFreeVirtualAddress(
        GetActiveVirtualMemoryInstance(),
        &tmp_loc_virt,
        size,
        MemoryAllocationType_Heap,
        MemoryAllocationFlags_Write);

    for(uint64_t i = 0; i < size; i += PAGE_SIZE) {

        uint64_t target_phys_addr = (uint64_t)GetPhysicalAddressPageTable(pageTable, (void*)(addr + i));
        uint64_t tmp_loc_phys = target_phys_addr/PAGE_SIZE * PAGE_SIZE;

        MapPage(GetActiveVirtualMemoryInstance(),
                tmp_loc_phys,
                tmp_loc_virt + i,
                PAGE_SIZE,
                CachingModeWriteBack,
                MemoryAllocationType_Heap,
                MemoryAllocationFlags_Write);
    }

    UnlockSpinlock(vmem_lock);
    return tmp_loc_virt;
}

void
UninstallTemporaryWriteMap(uint64_t loc,
                           size_t size) {
    LockSpinlock(vmem_lock);

    if(size % PAGE_SIZE)size += PAGE_SIZE - size % PAGE_SIZE;

    loc -= loc % PAGE_SIZE;
    UnmapPage(GetActiveVirtualMemoryInstance(),
              loc,
              size);

    UnlockSpinlock(vmem_lock);
}


void
WriteValueAtAddress64(ManagedPageTable *pageTable,
                      uint64_t *addr,
                      uint64_t val) {
    if((uint64_t)addr % 8 != 0)HaltProcessor();

    uint64_t target_phys_addr = (uint64_t)GetPhysicalAddressPageTable(pageTable, (void*)addr);
    //align the physical address to a page boundary and map it to the current page table

    LockSpinlock(vmem_lock);

    uint64_t tmp_loc_virt = 0;
    uint64_t tmp_loc_phys = target_phys_addr/PAGE_SIZE * PAGE_SIZE;

    //First allocate the same amount of memory in another location
    FindFreeVirtualAddress(
        GetActiveVirtualMemoryInstance(),
        &tmp_loc_virt,
        PAGE_SIZE,
        MemoryAllocationType_Heap,
        MemoryAllocationFlags_Write);

    uint64_t target_addr = tmp_loc_virt + (uint64_t)addr % PAGE_SIZE;

    MapPage(GetActiveVirtualMemoryInstance(),
            tmp_loc_phys,
            tmp_loc_virt,
            PAGE_SIZE,
            CachingModeWriteBack,
            MemoryAllocationType_Heap,
            MemoryAllocationFlags_Write);

    //Then copy over the data
    *(uint64_t*)target_addr = val;

    //Then undo the above mapping
    UnmapPage(GetActiveVirtualMemoryInstance(),
              tmp_loc_virt,
              PAGE_SIZE);



    UnlockSpinlock(vmem_lock);
}

void
WriteValueAtAddress32(ManagedPageTable *pageTable,
                      uint32_t *addr,
                      uint32_t val) {
    if((uint64_t)addr % 4 != 0)HaltProcessor();

    uint64_t target_phys_addr = (uint64_t)GetPhysicalAddressPageTable(pageTable, (void*)addr);
    //align the physical address to a page boundary and map it to the current page table

    LockSpinlock(vmem_lock);

    uint64_t tmp_loc_virt = 0;
    uint64_t tmp_loc_phys = target_phys_addr/PAGE_SIZE * PAGE_SIZE;

    //First allocate the same amount of memory in another location
    FindFreeVirtualAddress(
        GetActiveVirtualMemoryInstance(),
        &tmp_loc_virt,
        PAGE_SIZE,
        MemoryAllocationType_Heap,
        MemoryAllocationFlags_Write);

    uint64_t target_addr = tmp_loc_virt + (uint64_t)addr % PAGE_SIZE;

    MapPage(GetActiveVirtualMemoryInstance(),
            tmp_loc_phys,
            tmp_loc_virt,
            PAGE_SIZE,
            CachingModeWriteBack,
            MemoryAllocationType_Heap,
            MemoryAllocationFlags_Write);

    //Then copy over the data
    *(uint32_t*)target_addr = val;

    //Then undo the above mapping
    UnmapPage(GetActiveVirtualMemoryInstance(),
              tmp_loc_virt,
              PAGE_SIZE);



    UnlockSpinlock(vmem_lock);
}

void
WipeMemoryTypeFromTable(ManagedPageTable *pageTable,
                        MemoryAllocationType type) {
    //Walk the page table, unmapping anything that has the same allocation type
    LockSpinlock(pageTable->lock);

    MemoryAllocationsMap *map = pageTable->AllocationMap;
    while(map != NULL) {

        MemoryAllocationsMap *n = map->next;
        if(map->AllocationType == type) {
            UnmapPage(pageTable,
                      n->VirtualAddress,
                      n->Length);
        }
        map = n;
    }

    UnlockSpinlock(pageTable->lock);
}