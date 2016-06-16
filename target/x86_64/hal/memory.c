#include "memory.h"
#include "common/common.h"
#include "virt_mem_manager/virt_mem_manager.h"
#include "page_manager/phys_mem_manager.h"
#include "kmalloc/kmalloc.h"
#include "smp/smp.h"

void*
GetVirtualAddress(CachingMode c,
                  void *physicalAddress) {
    return VirtMemMan_GetVirtualAddress(c, physicalAddress);
}

void*
GetPhysicalAddress(void *virtualAddress) {
    return GetPhysicalAddressUID(GetActiveVirtualMemoryInstance(),
                                 virtualAddress);
}

void*
GetPhysicalAddressUID(UID 	src,
                      void 	*virtualAddress) {
    return VirtMemMan_GetPhysicalAddress((PML_Instance)src, virtualAddress);
}

MemoryAllocationErrors
CreateVirtualMemoryInstance(UID *inst) {
    if(inst != NULL) {
        *inst = (UID)VirtMemMan_CreateInstance();
        return MemoryAllocationErrors_None;
    }
    return MemoryAllocationErrors_Unknown;
}

UID
SetActiveVirtualMemoryInstance(UID inst) {
    return (UID)VirtMemMan_SetCurrent((PML_Instance)inst);
}

UID
GetActiveVirtualMemoryInstance(void) {
    return (UID)VirtMemMan_GetCurrent();
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

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
UnmapPage(UID 			pageTable,
          uint64_t 		virtualAddress,
          size_t 		size) {
    VirtMemMan_Unmap((PML_Instance)pageTable,
                     virtualAddress,
                     (uint64_t)size);

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
FindFreeVirtualAddress(UID 			pageTable,
                       uint64_t			*virtualAddress,
                       size_t 			size,
                       MemoryAllocationType 	allocType,
                       MemoryAllocationFlags 	flags) {

    if(virtualAddress == NULL)return MemoryAllocationErrors_Unknown;

    MEM_SECURITY_PERMS perms = 0;
    if(flags & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    if(flags & MemoryAllocationFlags_User)perms |= MEM_USER;

    void* addr = VirtMemMan_FindFreeAddress((PML_Instance)pageTable,
                                            size,
                                            allocType,
                                            perms);

    if(addr != NULL)*virtualAddress = (uint64_t)addr;

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
        if(c->AllocationType != MemoryAllocationType_Stack) {
            MapPage(*dst,
                    b,
                    (uint64_t)GetPhysicalAddressUID(src, (void*)c->VirtualAddress),
                    c->VirtualAddress,
                    c->Length,
                    c->CacheMode,
                    c->AllocationType,
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
    return MemMan_Alloc();
}

void
FreePhysicalPage(uint64_t ptr) {
    MemMan_Free(ptr);
}

uint64_t
AllocatePhysicalPageCont(int pageCount) {
    return MemMan_Alloc4KiBPageCont(pageCount);
}

void
FreePhysicalPageCont(uint64_t ptr,
                     int pageCount) {
    MemMan_FreeCont(ptr, pageCount);
}

void*
AllocateAPLSMemory(uint64_t size) {
    return VirtMemMan_AllocCoreLocalData(size);
}

int
GetCoreCount(void) {
    return SMP_GetCoreCount();
}