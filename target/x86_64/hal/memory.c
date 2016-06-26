#include "memory.h"
#include "common/common.h"
#include "virt_mem_manager/virt_mem_manager.h"
#include "page_manager/phys_mem_manager.h"
#include "kmalloc/kmalloc.h"
#include "smp/smp.h"
#include "synchronization.h"

static Spinlock vmem_lock = NULL;

void
MemoryHAL_Initialize(void) {
    vmem_lock = CreateBootstrapSpinlock();
}

void*
GetVirtualAddress(CachingMode c,
                  void *physicalAddress) {
    void* ret = VirtMemMan_GetVirtualAddress(c, physicalAddress);
    return ret;
}

void*
GetPhysicalAddress(void *virtualAddress) {
    void *ret = GetPhysicalAddressUID(GetActiveVirtualMemoryInstance(),
                                      virtualAddress);
    return ret;
}

void*
GetPhysicalAddressUID(UID 	src,
                      void 	*virtualAddress) {
    void *ret = VirtMemMan_GetPhysicalAddress((PML_Instance)src, virtualAddress);
    return ret;
}

MemoryAllocationErrors
CreateVirtualMemoryInstance(UID *inst) {
    if(inst != NULL) {
        LockSpinlock(vmem_lock);
        *inst = (UID)VirtMemMan_CreateInstance();
        UnlockSpinlock(vmem_lock);
        return MemoryAllocationErrors_None;
    }
    return MemoryAllocationErrors_Unknown;
}

void
FreeVirtualMemoryInstance(UID inst) {
    if(inst != 0 && inst % PAGE_SIZE == 0) {
        LockSpinlock(vmem_lock);
        VirtMemMan_FreePageTable((PML_Instance)inst);
        UnlockSpinlock(vmem_lock);
    }
}

UID
SetActiveVirtualMemoryInstance(UID inst) {
    LockSpinlock(vmem_lock);
    UID ret = (UID)VirtMemMan_SetCurrent((PML_Instance)inst);
    UnlockSpinlock(vmem_lock);
    return ret;
}

UID
GetActiveVirtualMemoryInstance(void) {
    UID ret = (UID)VirtMemMan_GetCurrent();
    return ret;
}

MemoryAllocationErrors
MapPage(UID 			pageTable,
        MemoryAllocationsMap	*allocationMap,
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


    //At this point, all parameters have been verified
    if(allocationMap != NULL) {
        allocationMap->CacheMode = cacheMode;
        allocationMap->VirtualAddress = virtualAddress;
        allocationMap->PhysicalAddress = physicalAddress;
        allocationMap->Length = size;
        allocationMap->Flags = flags;
        allocationMap->AllocationType = allocType;
        allocationMap->AdditionalData = 0;
        allocationMap->ReferenceCount = 0;
    }

    if(allocType & MemoryAllocationType_Fork) {
        access = access & ~MEM_WRITE; //Forked pages are copy on write
    }

    VirtMemMan_Map((PML_Instance)pageTable,
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
UnmapPage(UID 			pageTable,
          uint64_t 		virtualAddress,
          size_t 		size) {

    LockSpinlock(vmem_lock);
    VirtMemMan_Unmap((PML_Instance)pageTable,
                     virtualAddress,
                     (uint64_t)size);

    UnlockSpinlock(vmem_lock);
    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
FindFreeVirtualAddress(UID 			pageTable,
                       uint64_t			*virtualAddress,
                       size_t 			size,
                       MemoryAllocationType 	allocType,
                       MemoryAllocationFlags 	flags) {

    if(virtualAddress == NULL)return MemoryAllocationErrors_Unknown;

    LockSpinlock(vmem_lock);

    MEM_SECURITY_PERMS perms = 0;
    if(flags & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if(flags & MemoryAllocationFlags_User)perms |= MEM_USER;

    void* addr = VirtMemMan_FindFreeAddress((PML_Instance)pageTable,
                                            size,
                                            allocType,
                                            perms);

    if(addr != NULL)*virtualAddress = (uint64_t)addr;

    UnlockSpinlock(vmem_lock);

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
ForkTable(UID src,
          MemoryAllocationsMap *srcAllocBase,
          UID *dst,
          MemoryAllocationsMap **dstAllocBase) {
    if(dst == NULL)return MemoryAllocationErrors_Unknown;

    CreateVirtualMemoryInstance(dst);

    if(dstAllocBase == NULL)return MemoryAllocationErrors_Unknown;
    if(srcAllocBase == NULL)return MemoryAllocationErrors_Unknown;

    MemoryAllocationsMap *b = kmalloc(sizeof(MemoryAllocationsMap));
    *dstAllocBase = b;

    MemoryAllocationsMap *c = srcAllocBase;

    //TODO review this code to make sure it works
    while(c != NULL) {
        if((c->AllocationType & MemoryAllocationType_Stack) != MemoryAllocationType_Stack) {
            MapPage(*dst,
                    b,
                    (uint64_t)GetPhysicalAddressUID(src, (void*)c->VirtualAddress),
                    c->VirtualAddress,
                    c->Length,
                    c->CacheMode,
                    c->AllocationType | MemoryAllocationType_Fork,
                    c->Flags
                   );
        }

        if(c->next != NULL) {
            b->next = kmalloc(sizeof(MemoryAllocationsMap));
            b = b->next;
        }
        c = c->next;
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


//Lock the physical page to prevent modification
uint64_t
LockPageToUser(uint64_t virtualAddress) {
    LockSpinlock(vmem_lock);
    uint64_t val = VirtMemMan_LockPageToUser((void*)virtualAddress);
    UnlockSpinlock(vmem_lock);
    return val;
}

//Unlock the physical page to allow modification, if this was allowed
void
UnlockPageToUser(uint64_t virtualAddress,
                 uint64_t lockKey) {
    LockSpinlock(vmem_lock);
    VirtMemMan_UnlockPageToUser((void*)virtualAddress, lockKey);
    UnlockSpinlock(vmem_lock);
}

void
HandlePageFault(void *virtualAddress,
                MemoryAllocationFlags error) {

}

void
CheckAddressPermissions(UID pageTable,
                        uint64_t addr,
                        CachingMode *cacheMode,
                        MemoryAllocationFlags *flags) {
    MEM_TYPES cache = 0;
    MEM_ACCESS_PERMS access_perm = 0;
    MEM_SECURITY_PERMS sec_perm = 0;

    CachingMode c = 0;
    MemoryAllocationFlags a = 0;

    LockSpinlock(vmem_lock);
    VirtMemMan_CheckAddressPermissions((PML_Instance)pageTable, addr, &cache, &access_perm, &sec_perm);
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