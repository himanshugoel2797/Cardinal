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

static Spinlock vmem_lock = NULL;
static volatile ManagedPageTable **curPageTable = NULL;

void
MemoryHAL_Initialize(void) {
    vmem_lock = CreateBootstrapSpinlock();
    RegisterInterruptHandler(0xE, VirtMemMan_HandlePageFault);
    curPageTable = (volatile ManagedPageTable**)AllocateAPLSMemory(sizeof(ManagedPageTable**));
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
    void *ret = VirtMemMan_GetPhysicalAddress((PML_Instance)src->PageTable, virtualAddress);
    return ret;
}

MemoryAllocationErrors
CreateVirtualMemoryInstance(ManagedPageTable *inst) {
    if(inst != NULL) {
        LockSpinlock(vmem_lock);
        inst->PageTable = (UID)VirtMemMan_CreateInstance();
        UnlockSpinlock(vmem_lock);
        return MemoryAllocationErrors_None;
    }
    return MemoryAllocationErrors_Unknown;
}

void
FreeVirtualMemoryInstance(ManagedPageTable *inst) {
    if(inst != NULL && inst->PageTable != 0 && inst->PageTable % PAGE_SIZE == 0) {
        LockSpinlock(vmem_lock);
        //First free all memory we can free
        
        MemoryAllocationsMap* m = inst->AllocationMap;
        while(m != NULL)
        {
            MemoryAllocationsMap* n = m->next;

            uint64_t phys_addr = m->PhysicalAddress;
            uint64_t len = m->Length;
            MemoryAllocationType allocType = m->AllocationType;

            UnmapPage(inst,
                      m->VirtualAddress,
                      m->Length);
            if(!(allocType & MemoryAllocationType_Global))
            {

                //TODO make this function check shared memory status first before releasing its memory
            FreePhysicalPageCont(phys_addr, len / PAGE_SIZE);
        }
            m = n;
        }

        if(inst->AllocationMap != NULL)
            HaltProcessor();

        VirtMemMan_FreePageTable((PML_Instance)inst->PageTable);
        UnlockSpinlock(vmem_lock);
    }
}

ManagedPageTable*
SetActiveVirtualMemoryInstance(ManagedPageTable *inst) {
    *curPageTable = inst;
    LockSpinlock(vmem_lock);
    VirtMemMan_SetCurrent((PML_Instance)inst->PageTable);
    UnlockSpinlock(vmem_lock);
    return (ManagedPageTable*)*curPageTable;
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

    LockSpinlock(vmem_lock);

    MEM_ACCESS_PERMS access = 0;
    if(flags & MemoryAllocationFlags_Exec)access |= MEM_EXEC;
    if(flags & MemoryAllocationFlags_Write)access |= MEM_WRITE;

    MEM_SECURITY_PERMS perms = 0;
    if((flags & MemoryAllocationFlags_Kernel) == MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if((flags & MemoryAllocationFlags_User) == MemoryAllocationFlags_User)perms |= MEM_USER;

    MemoryAllocationsMap *allocMap = kmalloc(sizeof(MemoryAllocationsMap));
    if(allocMap == NULL)
        allocMap = bootstrap_malloc(sizeof(MemoryAllocationsMap));

    allocMap->CacheMode = cacheMode;
    allocMap->VirtualAddress = virtualAddress;
    allocMap->PhysicalAddress = physicalAddress;
    allocMap->Length = size;
    allocMap->Flags = flags;
    allocMap->AllocationType = allocType;
    allocMap->AdditionalData = 0;

    //At this point, all parameters have been verified
    allocMap->next = pageTable->AllocationMap;
    pageTable->AllocationMap = allocMap->next;

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

    UnlockSpinlock(vmem_lock);

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
UnmapPage(ManagedPageTable 	*pageTable,
          uint64_t 		     virtualAddress,
          size_t 		     size) {

    LockSpinlock(vmem_lock);

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
                top->PhysicalAddress = (uint64_t)GetPhysicalAddressPageTable(pageTable, (void*)(virtualAddress + size));
                top->Length = (map->VirtualAddress + map->Length) - (virtualAddress + size);

                map->Length = virtualAddress - map->VirtualAddress;

                if(top->Length != 0) {
                    top->next = map->next;
                    map->next = top;
                } else kfree(top);

                if(map->Length == 0) {
                    if(pageTable->AllocationMap != map)
                        prev->next = map->next;
                    else
                        pageTable->AllocationMap = map->next;

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

    UnlockSpinlock(vmem_lock);
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

    LockSpinlock(vmem_lock);

    MEM_SECURITY_PERMS perms = 0;
    if(flags & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if(flags & MemoryAllocationFlags_User)perms |= MEM_USER;

    void* addr = VirtMemMan_FindFreeAddress((PML_Instance)pageTable->PageTable,
                                            size,
                                            allocType,
                                            perms);

    if(addr != NULL)*virtualAddress = (uint64_t)addr;

    UnlockSpinlock(vmem_lock);

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
ForkTable(ManagedPageTable *src,
          ManagedPageTable *dst) {
    if(dst == NULL)return MemoryAllocationErrors_Unknown;

    CreateVirtualMemoryInstance(dst);

    if(src == NULL)return MemoryAllocationErrors_Unknown;
    if(dst == NULL)return MemoryAllocationErrors_Unknown;

    MemoryAllocationsMap *c = src->AllocationMap;

    //TODO review this code to make sure it works
    while(c != NULL) {
        if((c->AllocationType & MemoryAllocationType_Stack) != MemoryAllocationType_Stack) {
            MapPage(dst,
                    (uint64_t)GetPhysicalAddressPageTable(src, (void*)c->VirtualAddress),
                    c->VirtualAddress,
                    c->Length,
                    c->CacheMode,
                    c->AllocationType | MemoryAllocationType_Fork,
                    c->Flags
                   );
        }
    }

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

void
HandlePageFault(uint64_t virtualAddress,
                uint64_t instruction_pointer,
                MemoryAllocationFlags error) {
    error = 0;
    if(!ProcessSys_IsInitialized()) {
        while(1)debug_gfx_writeLine("Error: Page Fault");
    } else {
        //Check the current process's memory info table
        ProcessInformation *procInfo = NULL;
        GetProcessReference(GetCurrentProcessUID(), &procInfo);
        if(procInfo == NULL | procInfo->PageTable->AllocationMap == NULL) {
            __asm__("cli\n\thlt" :: "a"(instruction_pointer));
            //while(1)debug_gfx_writeLine("Error: Page Fault");
        }

        MemoryAllocationsMap *map = procInfo->PageTable->AllocationMap;
        do {
            if(virtualAddress >= map->VirtualAddress && virtualAddress <= (map->VirtualAddress + map->Length)) {
                //Found an entry that describes this fault
                if(map->AllocationType & MemoryAllocationType_PageLocked) {
                    //Yield the thread to the scheduler
                    TaskSwitch(0, 0);
                } else if(map->AllocationType & MemoryAllocationType_Application) {
                    __asm__("cli\n\thlt");
                }


                break;
            }

            map = map->next;
        } while(map != NULL);

    }
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

    LockSpinlock(vmem_lock);
    VirtMemMan_CheckAddressPermissions((PML_Instance)pageTable->PageTable, addr, &cache, &access_perm, &sec_perm);
    UnlockSpinlock(vmem_lock);

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
HaltProcessor(void)
{
    __asm__ volatile("cli\n\thlt");
}