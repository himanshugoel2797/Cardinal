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
static List *SharedMemoryUnits = NULL;
static volatile _Atomic uint64_t shmem_id_base = 1;

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
                ForkedMemoryData *fork_data = (ForkedMemoryData*)m->AdditionalData;
                SharedMemoryData *share_data = (SharedMemoryData*)m->AdditionalData;

                if(allocType & MemoryAllocationType_Fork) {
                    LockSpinlock(fork_data->Lock);
                    AtomicDecrement32(&fork_data->NetReferenceCount);
                }else if(allocType == MemoryAllocationType_Shared) {
                    LockSpinlock(share_data->Lock);
                    AtomicDecrement32(&share_data->NetReferenceCount);
                }

                UnmapPage(inst,
                          m->VirtualAddress,
                          m->Length);
                bool toFree = !(allocType & MemoryAllocationType_Global);
                bool forkFree = (allocType & MemoryAllocationType_Fork);
                forkFree = forkFree && (fork_data->NetReferenceCount == 0);

                bool shmemFree = (allocType == MemoryAllocationType_Shared);
                shmemFree = shmemFree && (share_data->NetReferenceCount == 0);

                if(toFree && (forkFree || shmemFree)) {
                    FreePhysicalPageCont(phys_addr, len / PAGE_SIZE);
                }

                if(forkFree) {
                    FreeSpinlock(fork_data->Lock);
                    kfree(fork_data);
                } else if(shmemFree) {

                    for(uint64_t i = 0; i < List_Length(SharedMemoryUnits); i++) {
                        if((uint64_t)share_data == (uint64_t)List_EntryAt(SharedMemoryUnits, i))
                            List_Remove(SharedMemoryUnits, i);
                    }

                    for(uint64_t i = 0; i < List_Length(share_data->Keys); i++) {
                        kfree(List_EntryAt(share_data->Keys, i));
                    }

                    List_Free(share_data->Keys);
                    FreeSpinlock(share_data->Lock);
                    kfree(share_data);
                }

                if(!forkFree && (allocType & MemoryAllocationType_Fork))
                    UnlockSpinlock(fork_data->Lock);

                if(!shmemFree && (allocType == MemoryAllocationType_Shared))
                    UnlockSpinlock(share_data->Lock);

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

    MemoryAllocationsMap *allocMap = NULL;
    if(((virtualAddress >> 39) & 0x1FF) != 511) {
        allocMap = kmalloc(sizeof(MemoryAllocationsMap));

        allocMap->CacheMode = cacheMode;
        allocMap->VirtualAddress = virtualAddress;
        allocMap->PhysicalAddress = physicalAddress;
        allocMap->Length = size;
        allocMap->Flags = flags;
        allocMap->AllocationType = allocType;
        allocMap->AdditionalData = 0;
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
InternalForkTable(ManagedPageTable *src,
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

        if(c->AllocationType != MemoryAllocationType_Shared){
        MapPage(dst,
                c->PhysicalAddress,
                c->VirtualAddress,
                c->Length,
                c->CacheMode,
                c->AllocationType | MemoryAllocationType_Fork,
                c->Flags
               );
        }

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

        while(src_map != NULL && src_map->AllocationType == MemoryAllocationType_Shared)src_map = src_map->next;
        if(src_map == NULL)
            HaltProcessor();

        if(src_map->VirtualAddress != dst_map->VirtualAddress)
            HaltProcessor();

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

static uint8_t tmp_stack[4096];

MemoryAllocationErrors
ForkTable(ManagedPageTable *src,
          ManagedPageTable *dst) {
    MemoryAllocationErrors err = 0;
    __asm__ volatile
    (
        "xchg %%rax, %%rsp\n\t"
        "push %%rax\n\t"
        "call InternalForkTable\n\t"
        "pop %%rcx\n\t"
        "mov %%rcx, %%rsp\n\t"
        :"=a"(err) :"a"(tmp_stack + 4096 - 16), "D"(src), "S"(dst) : "rcx"
    );
    return err;
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
                if(fork_data->NetReferenceCount == 0){
                    FreeSpinlock(fork_data->Lock);
                    kfree(fork_data);
                }

                break;

            }

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

static MemoryAllocationFlags 
TranslateSharedMemoryFlags(uint64_t flags) {
    MemoryAllocationFlags f = MemoryAllocationFlags_User | MemoryAllocationFlags_NoExec;

    if(flags & SharedMemoryFlags_Write)
        f |= MemoryAllocationFlags_Write;

    if(flags & SharedMemoryFlags_Read)
        f |= MemoryAllocationFlags_Read;

    if(flags & SharedMemoryFlags_Exec)
        f |= MemoryAllocationFlags_Exec;

    return f;
}

uint64_t
ManageSharedMemoryKey(size_t size, uint64_t flags, uint64_t key) {
    
    if(SharedMemoryUnits == NULL)
        SharedMemoryUnits = List_Create(CreateSpinlock());
    
    if(size % PAGE_SIZE)
        size += PAGE_SIZE - (size % PAGE_SIZE);

    if(flags & SharedMemoryFlags_Allocate) {
        static uint64_t shmem_addr_base = SHMEM_VADDR_BASE;

        SharedMemoryData *shmem_data = kmalloc(sizeof(SharedMemoryData));
        shmem_data->Keys = List_Create(CreateSpinlock());
        shmem_data->Permissions = TranslateSharedMemoryFlags(flags);
        shmem_data->MasterProcess = GetCurrentProcessUID();
        shmem_data->VirtualAddress = shmem_addr_base;
        shmem_data->PhysicalAddress = AllocatePhysicalPageCont(size/PAGE_SIZE);
        shmem_data->Length = size;
        shmem_data->MasterKey = (uint32_t)shmem_id_base++;
        shmem_data->NetReferenceCount = 0;

        shmem_addr_base += size;

        List_AddEntry(SharedMemoryUnits, shmem_data);
        return (uint64_t)shmem_data->MasterKey << 32;

    } else if(flags & SharedMemoryFlags_Free) {

    }else {

        for(uint64_t i = 0; i < List_Length(SharedMemoryUnits); i++) {
            SharedMemoryData *shmem_data = List_EntryAt(SharedMemoryUnits, i);

            if(shmem_data->MasterKey == (uint32_t)(key >> 32)) {

                //Make sure the current flags are a subset of the master flags
                //If they are, allocate a new key with the master key applied as metadata

                if(shmem_data->MasterProcess != GetCurrentProcessUID())
                    return -EPERM;

                MemoryAllocationFlags tmpFlags = TranslateSharedMemoryFlags(flags);

                if(~shmem_data->Permissions & tmpFlags)
                    return -EPERM;

                SharedMemoryKey *key_data = kmalloc(sizeof(SharedMemoryKey));
                key_data->Key = (uint32_t)shmem_id_base++;
                key_data->Permissions = tmpFlags;

                List_AddEntry(shmem_data->Keys, key_data);

                return key_data->Key | ((uint64_t)shmem_data->MasterKey << 32);
            }
        }

        return -EINVAL;
    }

    return -EINVAL;
}

uint64_t
SharedMemoryKeyAction(uint64_t key, uint64_t flags) {

    //Search the shared memory table and apply the mapping
    SharedMemoryData *memData = NULL;
    for(uint64_t i = 0; i < List_Length(SharedMemoryUnits); i++) {
        SharedMemoryData *tmpData = List_EntryAt(SharedMemoryUnits, i);

        LockSpinlock(tmpData->Lock);

        if(tmpData->MasterKey == (key >> 32)){
            memData = tmpData;
            break;
        }

        UnlockSpinlock(tmpData->Lock);
    }

    if(memData == NULL)
        return -EINVAL;

    MemoryAllocationFlags aFlags = memData->Permissions;
    uint32_t skey = (uint32_t)key;

    if(skey != 0) {
        uint64_t i = 0;
        for(i = 0; i < List_Length(memData->Keys); i++) {
            SharedMemoryKey *key_data = List_EntryAt(memData->Keys, i);

            if(key_data->Key == skey) {
                aFlags = key_data->Permissions;
                break;
            }
        }
        if(i == List_Length(memData->Keys))
            return UnlockSpinlock(memData->Lock), -EINVAL;
    }

    if((flags & SharedMemoryFlags_Map) == SharedMemoryFlags_Map){

        MapPage(GetActiveVirtualMemoryInstance(),
                memData->PhysicalAddress,
                memData->VirtualAddress,
                memData->Length,
                (flags & SharedMemoryFlags_Uncached)?CachingModeUncachable : CachingModeWriteBack,
                MemoryAllocationType_Shared,
                aFlags
                );
        ManagedPageTable* act = GetActiveVirtualMemoryInstance();
        LockSpinlock(act->lock);

        MemoryAllocationsMap *c = act->AllocationMap;
        while(c != NULL) {

            if(c->VirtualAddress == memData->VirtualAddress){
                c->AdditionalData = memData;
                break;
            }

            c = c->next;
        }

        memData->NetReferenceCount++;
        UnlockSpinlock(memData->Lock);
        return memData->VirtualAddress;

    }else if((flags & SharedMemoryFlags_Unmap) == SharedMemoryFlags_Unmap){

        UnmapPage(GetActiveVirtualMemoryInstance(),
                  memData->VirtualAddress,
                  memData->Length);

        memData->NetReferenceCount--;
        UnlockSpinlock(memData->Lock);
        return 0;
    }

    return -EINVAL;
}

void
WipeMemoryTypeFromTable(ManagedPageTable *pageTable,
                        MemoryAllocationType type)
{
    //Walk the page table, unmapping anything that has the same allocation type
}