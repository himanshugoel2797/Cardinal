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
        if(inst->PageTable != 0 && inst->PageTable % PAGE_SIZE == 0) {

            //First free all memory we can free

            MemoryAllocationsMap* m = inst->AllocationMap;
            while(m != NULL) {
                MemoryAllocationsMap* n = m->next;

                uint64_t phys_addr = m->PhysicalAddress;
                uint64_t len = m->Length;
                MemoryAllocationType allocType = m->AllocationType;

                UnmapPage(inst,
                          m->VirtualAddress,
                          m->Length);
                if(!(allocType & MemoryAllocationType_Global)) {

                    //TODO make this function check shared memory status first before releasing its memory
                    FreePhysicalPageCont(phys_addr, len / PAGE_SIZE);
                }
                m = n;
            }

            if(inst->AllocationMap != NULL)
                HaltProcessor();

            VirtMemMan_FreePageTable((PML_Instance)inst->PageTable);
        }
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

    MemoryAllocationsMap *allocMap = kmalloc(sizeof(MemoryAllocationsMap));

    allocMap->CacheMode = cacheMode;
    allocMap->VirtualAddress = virtualAddress;
    allocMap->PhysicalAddress = physicalAddress;
    allocMap->Length = size;
    allocMap->Flags = flags;
    allocMap->AllocationType = allocType;
    allocMap->AdditionalData = 0;

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
                top->AdditionalData = map->AdditionalData;

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
    allocMap->next = pageTable->AllocationMap;
    if(pageTable->AllocationMap != NULL)pageTable->AllocationMap->prev = allocMap;
    allocMap->prev = NULL;
    pageTable->AllocationMap = allocMap;

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
                top->AdditionalData = map->AdditionalData;

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

MemoryAllocationErrors
ForkTable(ManagedPageTable *src,
          ManagedPageTable *dst) {
    if(dst == NULL)return MemoryAllocationErrors_Unknown;

    dst->reference_count = 0;
    dst->lock = CreateSpinlock();
    dst->AllocationMap = NULL;
    CreateVirtualMemoryInstance(dst);

    if(src == NULL)return MemoryAllocationErrors_Unknown;
    if(dst == NULL)return MemoryAllocationErrors_Unknown;

    LockSpinlock(src->lock);
    MemoryAllocationsMap *c = src->AllocationMap;

    //TODO review this code to make sure it works
    while(c != NULL) {
        MapPage(dst,
                c->PhysicalAddress,
                c->VirtualAddress,
                c->Length,
                c->CacheMode,
                c->AllocationType | MemoryAllocationType_Fork,
                c->Flags
               );

        c = c->next;
    }

    c = dst->AllocationMap;
    MemoryAllocationsMap *tmp_node = NULL;

    while(c != NULL) {
        tmp_node = c->prev;
        c->prev = c->next;
        c->next = tmp_node;

        tmp_node = c;
        c = c->prev;
    }

    if(tmp_node != NULL)
        dst->AllocationMap = tmp_node;

    //Setup the fork data structure before marking the src data as forked in order to determine the fork source
    MemoryAllocationsMap *src_map = src->AllocationMap;
    MemoryAllocationsMap *dst_map = dst->AllocationMap;

    while(src_map != NULL && dst_map != NULL) {

        if(src_map->VirtualAddress != dst_map->VirtualAddress)
            __asm__ ("cli\n\thlt");

        if(src_map->AllocationType & MemoryAllocationType_Fork)
            dst_map->AdditionalData = src_map->AdditionalData;
        else {
            dst_map->AdditionalData = kmalloc(sizeof(ForkedMemoryData));
            ((ForkedMemoryData*)dst_map->AdditionalData)->Lock = CreateSpinlock();
        }

        ForkedMemoryData *fork_data = (ForkedMemoryData*)dst_map->AdditionalData;
        LockSpinlock(fork_data->Lock);
        fork_data->VirtualAddress = src_map->VirtualAddress;
        fork_data->PhysicalAddress = src_map->PhysicalAddress;
        fork_data->Length = src_map->Length;

        if(src_map->AllocationType & MemoryAllocationType_Fork)
            AtomicIncrement32(&fork_data->NetReferenceCount);
        else
            fork_data->NetReferenceCount = 2;

        fork_data->Flags = src_map->Flags;
        fork_data->AllocationType = src_map->AllocationType & ~MemoryAllocationType_Fork;
        fork_data->CacheMode = src_map->CacheMode;

        src_map->AdditionalData = fork_data;
        UnlockSpinlock(fork_data->Lock);

        src_map = src_map->next;
        dst_map = dst_map->next;
    }

    c = dst->AllocationMap;

    while(c != NULL) {

        ChangePageFlags(src,
                        c->VirtualAddress,
                        c->CacheMode,
                        c->AllocationType | MemoryAllocationType_Fork,
                        c->Flags
                       );

        c = c->next;
    }

    UnlockSpinlock(src->lock);

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
        if(virtualAddress >= map->VirtualAddress && virtualAddress <= (map->VirtualAddress + map->Length)) {
            //Found an entry that describes this fault
            if((map->AllocationType & MemoryAllocationType_Fork) && (error & MemoryAllocationFlags_Write)) {

                ForkedMemoryData *fork_data = (ForkedMemoryData*)map->AdditionalData;
                LockSpinlock(fork_data->Lock);

                uint64_t tmp_loc_virt = 0;
                uint64_t tmp_loc_phys = fork_data->NetReferenceCount-- > 1 ? AllocatePhysicalPageCont(fork_data->Length / PAGE_SIZE) : fork_data->PhysicalAddress;

                //First allocate the same amount of memory in another location
                FindFreeVirtualAddress(
                    procInfo->PageTable,
                    &tmp_loc_virt,
                    fork_data->Length,
                    MemoryAllocationType_Heap,
                    MemoryAllocationFlags_Write);

                MapPage(procInfo->PageTable,
                        tmp_loc_phys,
                        tmp_loc_virt,
                        fork_data->Length,
                        CachingModeWriteBack,
                        MemoryAllocationType_Heap,
                        MemoryAllocationFlags_Write);

                //Then copy over the data
                memcpy((void*)tmp_loc_virt, (void*)fork_data->VirtualAddress, fork_data->Length);

                //Then undo the above mapping
                UnmapPage(procInfo->PageTable,
                          tmp_loc_virt,
                          fork_data->Length);

                //Then map the same pages on top of the existing set
                UnmapPage(procInfo->PageTable,
                          fork_data->VirtualAddress,
                          fork_data->Length);

                MapPage(procInfo->PageTable,
                        tmp_loc_phys,
                        fork_data->VirtualAddress,
                        fork_data->Length,
                        fork_data->CacheMode,
                        fork_data->AllocationType,
                        fork_data->Flags
                       );

                map->AllocationType &= ~MemoryAllocationType_Fork;

                //Now cause a TLB shootdown
                PerformTLBShootdown();

                UnlockSpinlock(fork_data->Lock);
                if(fork_data->NetReferenceCount == 0)
                    kfree(fork_data);

                break;

            }

            if(map->AllocationType & MemoryAllocationType_Application) {
                __asm__("cli\n\thlt" :: "b"(3));
            }


            break;
        }

        MemoryAllocationsMap *tmp_next = map->next;

        map = tmp_next;
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
    while(tlb_core_count != GetCoreCount());
    tlb_core_count = 1;
    tlb_shootdown = FALSE;
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