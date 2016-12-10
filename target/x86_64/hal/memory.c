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
static MemoryAllocationsMap *KernelMap = NULL;

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
        LockSpinlock(vmem_lock);
        inst->PageTable = (UID)VirtMemMan_CreateInstance();
        inst->lock = CreateSpinlock();
        inst->reference_count = 1;

        inst->UserMap = NULL;

        UnlockSpinlock(vmem_lock);
        return MemoryAllocationErrors_None;
    }
    return MemoryAllocationErrors_Unknown;
}

void
FreeVirtualMemoryInstance(ManagedPageTable *inst) {
    if(inst != NULL) {
        LockSpinlock(vmem_lock);

        //The kernel expects the user mode to have freed up any and all memory as needed
        LockSpinlock(inst->lock);

        for(int i = MAX_ALLOCATION_TYPE_BIT; i >= 0; i--) {
            WipeMemoryTypeFromTable(inst, 1 << i);
        }

        VirtMemMan_FreePageTable((PML_Instance)inst->PageTable);

        UnlockSpinlock(inst->lock);
        UnlockSpinlock(vmem_lock);
    }
}

extern int ts_inited;

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

    //If this allocation is just reserved vmem, mark the page as not present
    if(allocType & MemoryAllocationType_ReservedAllocation) {

        if(allocType & MemoryAllocationType_Shared)
            return MemoryAllocationErrors_InvalidFlags;

        flags &= ~MemoryAllocationFlags_Present;
        flags |= MemoryAllocationFlags_NotPresent;

        //In this case, error out if a physical address is specified,
        //backing memory is purely runtime allocated for this
        if(physicalAddress != 0)
            return MemoryAllocationErrors_InvalidFlags;
    }

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

    allocMap = kmalloc(sizeof(MemoryAllocationsMap));

    allocMap->CacheMode = cacheMode;
    allocMap->VirtualAddress = virtualAddress;
    allocMap->PhysicalAddress = physicalAddress;
    allocMap->Length = size;
    allocMap->Flags = flags;
    allocMap->AllocationType = allocType;

    LockSpinlock(pageTable->lock);

    if(pageTable->UserMap != NULL && KernelMap != NULL) {

        MemoryAllocationsMap *map = NULL;
        MemoryAllocationsMap *prev = NULL;
        MemoryAllocationsMap **map_root = NULL;

        if(flags & MemoryAllocationFlags_Kernel) {
            map = KernelMap;
            map_root = &KernelMap;
        } else if(flags & MemoryAllocationFlags_User) {
            map = pageTable->UserMap;
            map_root = &pageTable->UserMap;
        } else
            __asm__("cli\n\thlt");

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

                    if(map->next != NULL)
                        map->next->prev = top;

                    map->next = top;
                } else kfree(top);

                if(map->Length == 0) {
                    if(*map_root != map)
                        prev->next = map->next;
                    else
                        *map_root = map->next;

                    if(map->next != NULL)
                        map->next->prev = prev;

                    kfree(map);
                }
                break;
            }

            prev = map;
            map = map->next;
        } while(map != NULL);
    }


    //If this is a kernel mapping, append it to the end of the list
    //Else put it at the start
    if(flags & MemoryAllocationFlags_User) {
        allocMap->next = pageTable->UserMap;
        allocMap->prev = NULL;
        pageTable->UserMap = allocMap;

    } else if(flags * MemoryAllocationFlags_Kernel) {
        allocMap->next = KernelMap;
        allocMap->prev = NULL;
        KernelMap = allocMap;
    }

    VirtMemMan_Map((PML_Instance)pageTable->PageTable,
                   virtualAddress,
                   physicalAddress,
                   size,
                   (flags & MemoryAllocationFlags_Present),
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

    MemoryAllocationsMap *map = NULL;
    if(flags & MemoryAllocationFlags_User)
        map = pageTable->UserMap;
    else if(flags & MemoryAllocationFlags_Kernel)
        map = KernelMap;

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

    VirtMemMan_Map((PML_Instance)pageTable->PageTable,
                   virtualAddress,
                   map->PhysicalAddress,
                   map->Length,
                   (flags & MemoryAllocationFlags_Present),
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


    if(pageTable->UserMap != NULL && KernelMap != NULL) {

        MemoryAllocationFlags flags = 0;
        GetAddressPermissions(pageTable, virtualAddress, NULL, &flags, NULL);

        MemoryAllocationsMap *map = NULL;
        MemoryAllocationsMap *prev = NULL;
        MemoryAllocationsMap **map_root = NULL;

        if(flags & MemoryAllocationFlags_Kernel) {
            map = KernelMap;
            map_root = &KernelMap;
        } else if(flags & MemoryAllocationFlags_User) {
            map = pageTable->UserMap;
            map_root = &pageTable->UserMap;
        } else
            __asm__("cli\n\thlt");

        do {

            if(map->VirtualAddress <= virtualAddress && (map->VirtualAddress + map->Length) >= (virtualAddress + size)) {

                //Shared memory explicitly requires full length match!
                if(size != map->Length && (map->AllocationType & MemoryAllocationType_Shared))
                    return MemoryAllocationErrors_InvalidFlags;

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

                    if(map->next != NULL)
                        map->next->prev = top;

                    map->next = top;
                } else kfree(top);

                if(map->Length == 0) {

                    if(map->AllocationType & MemoryAllocationType_Shared) {
                        map->SharedMemoryInfo->ReferenceCount--;

                        if(map->SharedMemoryInfo->ReferenceCount == 0) {
                            //Delete the mapping and free the memory
                            //TODO Is this a good idea? Freeing shouldn't be mixed in here if allocation isn't
                            FreePhysicalPageCont(map->SharedMemoryInfo->PhysicalAddress, map->SharedMemoryInfo->Length / PAGE_SIZE);
                            kfree(map->SharedMemoryInfo);
                        }
                    }

                    if(*map_root != map)
                        prev->next = map->next;
                    else
                        *map_root = map->next;

                    if(map->next != NULL)
                        map->next->prev = map->prev;

                    kfree(map);
                }
                break;
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


    //Ignore the reserved allocation flag here, it only matters on map, unmap and #PF
    if(allocType & MemoryAllocationType_ReservedAllocation)
        allocType = allocType & ~MemoryAllocationType_ReservedAllocation;

    uint64_t addr = 0;

    if(VirtMemMan_FindFreeAddress((PML_Instance)pageTable->PageTable,
                                  &addr,
                                  size,
                                  allocType,
                                  perms) != 0)
        return MemoryAllocationErrors_Unknown;

    if(addr != 0)*virtualAddress = addr;

    UnlockSpinlock(pageTable->lock);

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
GetPageSize(ManagedPageTable *pageTable,
            uint64_t virtualAddress,
            uint64_t *pageSize) {
    if(pageTable == NULL | pageSize == NULL)
        return MemoryAllocationErrors_Unknown;

    LockSpinlock(pageTable->lock);

    uint64_t result = VirtMemMan_GetPageSize((PML_Instance)pageTable->PageTable,
                      virtualAddress);

    UnlockSpinlock(pageTable->lock);

    if(result == (uint64_t)-1)
        return MemoryAllocationErrors_InvalidVirtualAddress;

    *pageSize = result;
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

MemoryAllocationErrors
MakeReservationReal(ManagedPageTable *pageTable,
                    uint64_t virtualAddress) {
    if(pageTable == NULL)
        return MemoryAllocationErrors_Unknown;

    LockSpinlock(pageTable->lock);

    uint64_t aligned_vaddr = virtualAddress & PAGE_ALIGN_MASK;
    MemoryAllocationFlags AllocationFlags = 0;
    MemoryAllocationType AllocationType = 0;
    CachingMode CacheMode = 0;
    GetAddressPermissions(pageTable, aligned_vaddr, &CacheMode, &AllocationFlags, &AllocationType);

    uint64_t page_size = 0;


    if(AllocationType & MemoryAllocationType_ReservedAllocation && GetPageSize(pageTable,
            aligned_vaddr,
            &page_size) == MemoryAllocationErrors_None) {

        //Give this page actual backing memory!
        uint64_t phys_addr = AllocatePhysicalPageCont(page_size / PAGE_SIZE);

        //Mark the pages as present
        AllocationFlags &= ~MemoryAllocationFlags_NotPresent;
        AllocationFlags |= MemoryAllocationFlags_Present;

        //Remove the reserved flag
        AllocationType &= ~MemoryAllocationType_ReservedAllocation;
        AllocationType |= MemoryAllocationType_ReservedBacking;

        //Unmap to update this entry correctly
        UnmapPage(pageTable,
                  aligned_vaddr,
                  page_size);

        //Map with actual memory
        MapPage(pageTable,
                phys_addr,
                aligned_vaddr,
                page_size,
                CacheMode,
                AllocationType,
                AllocationFlags);

        UnlockSpinlock(pageTable->lock);
        return MemoryAllocationErrors_None;
    }
    UnlockSpinlock(pageTable->lock);
    return MemoryAllocationErrors_Unknown;
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
                MemoryAllocationFlags UNUSED(error)) {

    if(!ProcessSys_IsInitialized()) {
        __asm__("cli\n\thlt" :: "a"(instruction_pointer));
    }
    //Check the current process's memory info table
    ProcessInformation *procInfo = NULL;
    GetProcessReference(GetCurrentProcessUID(), &procInfo);
    if(procInfo == NULL | KernelMap == NULL | procInfo->PageTable->UserMap == NULL) {
        __asm__("cli\n\thlt" :: "a"(instruction_pointer), "b"(1), "c"(procInfo->ID));
        //while(1)debug_gfx_writeLine("Error: Page Fault");
    }

    uint64_t aligned_vaddr = virtualAddress & PAGE_ALIGN_MASK;

    LockSpinlock(procInfo->lock);
    LockSpinlock(procInfo->PageTable->lock);


    MemoryAllocationFlags alloc_perms = 0;
    GetAddressPermissions(procInfo->PageTable, virtualAddress, NULL, &alloc_perms, NULL);

    MemoryAllocationsMap *map = NULL;
    if(alloc_perms & MemoryAllocationFlags_User)
        map = procInfo->PageTable->UserMap;
    else if(alloc_perms & MemoryAllocationFlags_Kernel)
        map = KernelMap;
    else
        __asm__("cli\n\thlt" :: "a"(instruction_pointer), "b"(virtualAddress), "c"(GetCurrentProcessUID()));

    while(map != NULL) {
        if(virtualAddress >= map->VirtualAddress && virtualAddress < (map->VirtualAddress + map->Length)) {

            //Found an entry that describes this fault
            if(map->AllocationType & MemoryAllocationType_ReservedAllocation) {
                MakeReservationReal(procInfo->PageTable, aligned_vaddr);
                break;
            }

//            if(map->AllocationType & MemoryAllocationType_Application) {
//            }


            break;
        }

        MemoryAllocationsMap *tmp_next = map->next;

        map = tmp_next;
    }

    if(map == NULL) {
        __asm__("cli\n\thlt" :: "a"(instruction_pointer), "b"(virtualAddress), "c"(GetCurrentProcessUID()));
    }
    UnlockSpinlock(procInfo->PageTable->lock);
    UnlockSpinlock(procInfo->lock);
}

void
GetAddressPermissions(ManagedPageTable      *pageTable,
                      uint64_t              addr,
                      CachingMode           *cacheMode,
                      MemoryAllocationFlags *flags,
                      MemoryAllocationType *allocType) {
    CachingMode c = 0;
    MemoryAllocationFlags a = 0;
    MemoryAllocationType t = 0;

    LockSpinlock(pageTable->lock);

    MemoryAllocationsMap *map = KernelMap;

    for(int i = 0; i < 2; i++) {
        do {
            if(addr >= map->VirtualAddress && addr < (map->VirtualAddress + map->Length)) {
                t = map->AllocationType;
                c = map->CacheMode;
                a = map->Flags;
                break;
            }
            map = map->next;
        } while(map != NULL);

        if(t == 0 && c == 0 && a == 0 && pageTable->UserMap != NULL) {
            map = pageTable->UserMap;
        } else
            break;
    }

    UnlockSpinlock(pageTable->lock);

    if(allocType != NULL)*allocType = t;
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

        if(target_phys_addr == 0) {
            uint64_t aligned_vaddr = (addr + i);
            MemoryAllocationFlags AllocationFlags = 0;
            MemoryAllocationType AllocationType = 0;
            CachingMode CacheMode = 0;
            GetAddressPermissions(pageTable, aligned_vaddr, &CacheMode, &AllocationFlags, &AllocationType);
            if(AllocationType & MemoryAllocationType_ReservedAllocation)
                MakeReservationReal(pageTable, addr + i);

            target_phys_addr = (uint64_t)GetPhysicalAddressPageTable(pageTable, (void*)(addr + i));
        }
        uint64_t tmp_loc_phys = target_phys_addr/PAGE_SIZE * PAGE_SIZE;

        MapPage(GetActiveVirtualMemoryInstance(),
                tmp_loc_phys,
                tmp_loc_virt + i,
                PAGE_SIZE,
                CachingModeWriteBack,
                MemoryAllocationType_Heap,
                MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Write | MemoryAllocationFlags_Present);
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

    LockSpinlock(vmem_lock);

    uint64_t tmp_loc_virt = SetupTemporaryWriteMap(pageTable, *addr, PAGE_SIZE);
    uint64_t target_addr = tmp_loc_virt + (uint64_t)addr % PAGE_SIZE;

    //Then copy over the data
    *(uint64_t*)target_addr = val;

    //Then undo the above mapping
    UninstallTemporaryWriteMap(tmp_loc_virt, PAGE_SIZE);

    UnlockSpinlock(vmem_lock);
}

void
WriteValueAtAddress32(ManagedPageTable *pageTable,
                      uint32_t *addr,
                      uint32_t val) {
    if((uint64_t)addr % 4 != 0)HaltProcessor();

    LockSpinlock(vmem_lock);

    uint64_t tmp_loc_virt = SetupTemporaryWriteMap(pageTable, *addr, PAGE_SIZE);
    uint64_t target_addr = tmp_loc_virt + (uint64_t)addr % PAGE_SIZE;

    //Then copy over the data
    *(uint32_t*)target_addr = val;

    //Then undo the above mapping
    UninstallTemporaryWriteMap(tmp_loc_virt, PAGE_SIZE);

    UnlockSpinlock(vmem_lock);
}

void
WipeMemoryTypeFromTable(ManagedPageTable *pageTable,
                        MemoryAllocationType type) {

    //Don't operate on flags
    if(type & (MemoryAllocationType_ReservedAllocation | MemoryAllocationType_ReservedBacking))
        return;

    //Walk the page table, unmapping anything that has the same allocation type
    LockSpinlock(pageTable->lock);

    MemoryAllocationsMap *map = pageTable->UserMap;
    while(map != NULL) {

        MemoryAllocationsMap *n = map->next;

        MemoryAllocationType allocType = map->AllocationType;

        //Ignore extra flags during this process.
        allocType &= ~(MemoryAllocationType_ReservedBacking | MemoryAllocationType_ReservedAllocation);

        if(allocType == type) {

            if(map->AllocationType & MemoryAllocationType_ReservedBacking) {
                FreePhysicalPageCont(map->PhysicalAddress, map->Length / PAGE_SIZE);
            }

            if(!(map->AllocationType & MemoryAllocationType_ReservedAllocation)) {
                UnmapPage(pageTable,
                          map->VirtualAddress,
                          map->Length);
            }

        }
        map = n;

    }

    UnlockSpinlock(pageTable->lock);
}

MemoryAllocationErrors
AllocateSharedMemory(UID pid,
                     uint64_t length,
                     CachingMode cacheMode,
                     MemoryAllocationType allocType,
                     MemoryAllocationFlags flags,
                     uint64_t *virtualAddress) {
    uint64_t mem = AllocatePhysicalPageCont(length / PAGE_SIZE);
    MemoryAllocationErrors err = AllocateSharedMemoryPhys(pid,
                                 length,
                                 cacheMode,
                                 allocType,
                                 flags,
                                 mem,
                                 virtualAddress);

    if(err != MemoryAllocationErrors_None)
        FreePhysicalPageCont(mem, length / PAGE_SIZE);

    return err;
}

MemoryAllocationErrors
AllocateSharedMemoryPhys(UID pid,
                         uint64_t length,
                         CachingMode cacheMode,
                         MemoryAllocationType allocType,
                         MemoryAllocationFlags flags,
                         uint64_t physicalAddress,
                         uint64_t *virtualAddress) {

    if(virtualAddress == NULL)
        return MemoryAllocationErrors_InvalidParameters;

    if((length % PAGE_SIZE) | (length == 0))
        return MemoryAllocationErrors_InvalidParameters;

    if(allocType & (MemoryAllocationType_ReservedAllocation | MemoryAllocationType_ReservedBacking))
        return MemoryAllocationErrors_InvalidFlags;

    ProcessInformation *procInfo = NULL;
    if(GetProcessReference(pid, &procInfo) != ProcessErrors_None)
        return MemoryAllocationErrors_InvalidParameters;


    LockSpinlock(procInfo->lock);
    LockSpinlock(procInfo->PageTable->lock);

    FindFreeVirtualAddress(
        procInfo->PageTable,
        virtualAddress,
        length,
        allocType,
        flags);

    if(*virtualAddress == 0) {
        UnlockSpinlock(procInfo->PageTable->lock);
        UnlockSpinlock(procInfo->lock);
        return MemoryAllocationErrors_OutOfMemory;
    }

    uint64_t mem = physicalAddress;
    if(mem == 0) {
        UnlockSpinlock(procInfo->PageTable->lock);
        UnlockSpinlock(procInfo->lock);
        return MemoryAllocationErrors_OutOfMemory;
    }

    MapPage(procInfo->PageTable,
            mem,
            *virtualAddress,
            length,
            cacheMode,
            allocType,
            flags | MemoryAllocationFlags_Present);

    MemoryAllocationsMap *map = NULL;
    if(flags & MemoryAllocationFlags_Kernel)
        map = KernelMap;
    else if(flags & MemoryAllocationFlags_User)
        map = procInfo->PageTable->UserMap;

    while(map != NULL) {
        if(*virtualAddress == map->VirtualAddress && length == map->Length) {

            map->AllocationType |= MemoryAllocationType_Shared;

            map->SharedMemoryInfo = kmalloc(sizeof(SharedMemoryData));
            map->SharedMemoryInfo->PhysicalAddress = mem;
            map->SharedMemoryInfo->Length = length;
            map->SharedMemoryInfo->ReferenceCount = 1;

            break;
        }
        map = map->next;
    }

    UnlockSpinlock(procInfo->PageTable->lock);
    UnlockSpinlock(procInfo->lock);

    if(map == NULL)
        return MemoryAllocationErrors_Unknown;

    return MemoryAllocationErrors_None;

}

MemoryAllocationErrors
GetSharedMemoryKey(UID pid,
                   uint64_t virtualAddress,
                   uint64_t length,
                   CachingMode cacheMode,
                   MemoryAllocationFlags flags,
                   uint64_t *key) {

    if(key == NULL)
        return MemoryAllocationErrors_InvalidParameters;

    ProcessInformation *procInfo = NULL;
    if(GetProcessReference(pid, &procInfo) != ProcessErrors_None)
        return MemoryAllocationErrors_InvalidParameters;

    LockSpinlock(procInfo->lock);
    LockSpinlock(procInfo->PageTable->lock);

    MemoryAllocationsMap *map = NULL;
    if(flags & MemoryAllocationFlags_Kernel)
        map = KernelMap;
    else if(flags & MemoryAllocationFlags_User)
        map = procInfo->PageTable->UserMap;

    while(map != NULL) {
        if(virtualAddress == map->VirtualAddress && length == map->Length) {


            if(!(map->AllocationType & MemoryAllocationType_Shared)) {
                UnlockSpinlock(procInfo->PageTable->lock);
                UnlockSpinlock(procInfo->lock);
                return MemoryAllocationErrors_InvalidParameters;
            }

            //Store the address of the mapping info and the flags in a key
            uint64_t identifiers[IDENTIFIER_COUNT];
            identifiers[0] = (uint64_t)map->SharedMemoryInfo;
            identifiers[1] = (uint64_t)flags;
            identifiers[2] = (uint64_t)cacheMode;
            identifiers[3] = 0;
            identifiers[IDENTIFIER_COUNT - 1] = KeyType_SharedMemoryKey;

            if(KeyMan_AllocateKey(identifiers, key) != KeyManagerErrors_None) {
                UnlockSpinlock(procInfo->PageTable->lock);
                UnlockSpinlock(procInfo->lock);
                return MemoryAllocationErrors_Unknown;
            }
            if(AllocateDescriptor(pid, *key, NULL) != ProcessErrors_None) {
                KeyMan_FreeKey(*key);
                UnlockSpinlock(procInfo->PageTable->lock);
                UnlockSpinlock(procInfo->lock);
                return MemoryAllocationErrors_OutOfMemory;
            }

            break;
        }
        map = map->next;
    }
    UnlockSpinlock(procInfo->PageTable->lock);
    UnlockSpinlock(procInfo->lock);

    if(map == NULL)
        return MemoryAllocationErrors_InvalidVirtualAddress;

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
ApplySharedMemoryKey(UID pid,
                     uint64_t key,
                     uint64_t *virtualAddress,
                     MemoryAllocationFlags *flags,
                     CachingMode *cacheMode,
                     uint64_t *length) {

    if(length == NULL)
        return MemoryAllocationErrors_InvalidParameters;

    if(cacheMode == NULL)
        return MemoryAllocationErrors_InvalidParameters;

    if(flags == NULL)
        return MemoryAllocationErrors_InvalidParameters;

    if(virtualAddress == NULL)
        return MemoryAllocationErrors_InvalidParameters;

    uint64_t identifiers[IDENTIFIER_COUNT];
    if(KeyMan_ReadKey(key, identifiers) != KeyManagerErrors_None)
        return MemoryAllocationErrors_InvalidParameters;

    if(identifiers[IDENTIFIER_COUNT - 1] != KeyType_SharedMemoryKey)
        return MemoryAllocationErrors_InvalidParameters;

    ProcessInformation *procInfo = NULL;
    if(GetProcessReference(pid, &procInfo) != ProcessErrors_None)
        return MemoryAllocationErrors_InvalidParameters;


    SharedMemoryData *shmem_info = (SharedMemoryData*)identifiers[0];

    identifiers[3]++;

    *flags = (MemoryAllocationFlags)identifiers[1];
    *cacheMode = (CachingMode)identifiers[2];
    *length = shmem_info->Length;

    LockSpinlock(procInfo->lock);
    LockSpinlock(procInfo->PageTable->lock);

    FindFreeVirtualAddress(
        procInfo->PageTable,
        virtualAddress,
        *length,
        MemoryAllocationType_MMap,
        *flags);

    if(*virtualAddress == 0) {
        UnlockSpinlock(procInfo->PageTable->lock);
        UnlockSpinlock(procInfo->lock);
        return MemoryAllocationErrors_OutOfMemory;
    }

    MapPage(procInfo->PageTable,
            shmem_info->PhysicalAddress,
            *virtualAddress,
            *length,
            *cacheMode,
            MemoryAllocationType_MMap,
            *flags | MemoryAllocationFlags_Present);

    MemoryAllocationsMap *map = NULL;
    if(*flags & MemoryAllocationFlags_Kernel)
        map = KernelMap;
    else if(*flags & MemoryAllocationFlags_User)
        map = procInfo->PageTable->UserMap;

    while(map != NULL) {
        if(*virtualAddress == map->VirtualAddress && *length == map->Length) {

            map->AllocationType |= MemoryAllocationType_Shared;

            map->SharedMemoryInfo = shmem_info;
            map->SharedMemoryInfo->ReferenceCount++;

            break;
        }
        map = map->next;
    }

    KeyMan_WriteKey(key, identifiers);

    UnlockSpinlock(procInfo->PageTable->lock);
    UnlockSpinlock(procInfo->lock);
    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
GetSharedMemoryKeyUsageCount(uint64_t key,
                             uint64_t *cnt) {

    if(cnt == NULL)
        return MemoryAllocationErrors_InvalidParameters;

    uint64_t identifiers[IDENTIFIER_COUNT];
    if(KeyMan_ReadKey(key, identifiers) != KeyManagerErrors_None)
        return MemoryAllocationErrors_InvalidParameters;

    if(identifiers[IDENTIFIER_COUNT - 1] != KeyType_SharedMemoryKey)
        return MemoryAllocationErrors_InvalidParameters;

    *cnt = identifiers[3];
    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
FreeSharedMemoryKey(UID parentPID,
                    uint64_t key) {

    uint32_t index = 0;

    if(GetIndexOfKey(parentPID, key, &index) != ProcessErrors_None)
        return MemoryAllocationErrors_InvalidParameters;

    uint64_t identifiers[IDENTIFIER_COUNT];
    if(KeyMan_ReadKey(key, identifiers) != KeyManagerErrors_None)
        return MemoryAllocationErrors_InvalidParameters;

    if(identifiers[IDENTIFIER_COUNT - 1] != KeyType_SharedMemoryKey)
        return MemoryAllocationErrors_InvalidParameters;

    if(DeleteDescriptor(parentPID, index) != ProcessErrors_None)
        return MemoryAllocationErrors_InvalidParameters;

    return MemoryAllocationErrors_None;
}