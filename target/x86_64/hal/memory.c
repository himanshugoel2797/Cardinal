#include "memory.h"
#include "virt_mem_manager/virt_mem_manager.h"
#include "page_manager/phys_mem_manager.h"

void*
GetVirtualAddress(CachingMode c,
                  void *physicalAddress)
{
    return VirtMemMan_GetVirtualAddress(c, physicalAddress);
}

void*
GetPhysicalAddress(void *virtualAddress)
{
    return VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), virtualAddress);
}

MemoryAllocationErrors
CreateVirtualMemoryInstance(UID *inst)
{
    if(inst != NULL)
        {
            *inst = (UID)VirtMemMan_CreateInstance();
            return MemoryAllocationErrors_None;
        }
    return MemoryAllocationErrors_Unknown;
}

UID
SetActiveVirtualMemoryInstance(UID inst)
{
    return (UID)VirtMemMan_SetCurrent((PML_Instance)inst);
}

UID
GetActiveVirtualMemoryInstance(void)
{
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
        MemoryAllocationFlags 	flags)
{

    MEM_TYPES cache = 0;
    if(cacheMode == CachingModeUncachable)cache = MEM_TYPE_UC;
    else if(cacheMode == CachingModeWriteBack)cache = MEM_TYPE_WB;
    else if(cacheMode == CachingModeWriteThrough)cache = MEM_TYPE_WT;
    else return MemoryAllocationErrors_InvalidFlags;

    MEM_ACCESS_PERMS access = 0;
    if(flags & MemoryAllocationFlags_Exec)access |= MEM_EXEC;
    if(flags & MemoryAllocationFlags_Write)access |= MEM_WRITE;

    MEM_SECURITY_PERMS perms = 0;
    if(flags & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    else if(flags & MemoryAllocationFlags_User)perms |= MEM_USER;

    //At this point, all parameters have been verified
    if(allocationMap != NULL)
        {
            allocationMap->CacheMode = cacheMode;
            allocationMap->VirtualAddress = virtualAddress;
            allocationMap->Length = size;
            allocationMap->Flags = flags;
            allocationMap->AllocationType = allocType;
            allocationMap->AdditionalData = 0;
            allocationMap->ReferenceCount = 0;
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
          MemoryAllocationsMap* UNUSED(allocationMap),
          uint64_t 		virtualAddress,
          size_t 		size)
{
    VirtMemMan_Map((PML_Instance)pageTable,
                   virtualAddress,
                   0,
                   size,
                   FALSE,
                   MEM_TYPE_UC,
                   0,
                   MEM_KERNEL
                  );

    return MemoryAllocationErrors_None;
}

MemoryAllocationErrors
FindFreeVirtualAddress(UID 			pageTable,
                       uint64_t			*virtualAddress,
                       size_t 			size,
                       MemoryAllocationType 	UNUSED(allocType),
                       MemoryAllocationFlags 	flags)
{

    if(virtualAddress == NULL)return MemoryAllocationErrors_Unknown;

    MEM_SECURITY_PERMS perms = 0;
    if(flags & MemoryAllocationFlags_Kernel)perms |= MEM_KERNEL;
    else if(flags & MemoryAllocationFlags_User)perms |= MEM_USER;

    void* addr = VirtMemMan_FindFreeAddress((PML_Instance)pageTable,
                                            size,
                                            perms);

    if(addr != NULL)*virtualAddress = (uint64_t)addr;

    return MemoryAllocationErrors_None;
}

uint64_t
AllocatePhysicalPage(void)
{
    return MemMan_Alloc();
}

void
FreePhysicalPage(uint64_t ptr)
{
    MemMan_Free(ptr);
}
