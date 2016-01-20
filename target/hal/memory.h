#ifndef _CARDINAL_HAL_MEMORY_OPS_H_
#define _CARDINAL_HAL_MEMORY_OPS_H_

#include "types.h"

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
#define PAGE_SIZE KiB(4)

typedef enum
{
    CachingModeWriteBack = 0,
    CachingModeUncachable = 1,
    CachingModeWriteThrough = 2
} CachingMode;

typedef enum
{
    MemoryAllocationType_Heap,
    MemoryAllocationType_Stack,
    MemoryAllocationType_MMap,
    MemoryAllocationType_Fork,
    MemoryAllocationType_Paged,
    MemoryAllocationType_Shared
} MemoryAllocationType;

typedef enum
{
    MemoryAllocationFlags_NoExec = (1 << 0),
    MemoryAllocationFlags_Exec = (1 << 1),
    MemoryAllocationFlags_Write = (1 << 2),
    MemoryAllocationFlags_Kernel = 0,
    MemoryAllocationFlags_User = (1 << 3)
} MemoryAllocationFlags;

typedef enum
{
    MemoryAllocationErrors_None = 0,
    MemoryAllocationErrors_AlreadyMapped = (1 << 0),
    MemoryAllocationErrors_NotMapped = (1 << 1),
    MemoryAllocationErrors_PartiallyMapped = (1 << 2),
    MemoryAllocationErrors_InvalidFlags = (1 << 3),
    MemoryAllocationErrors_InvalidVirtualAddress = (1 << 4),
    MemoryAllocationErrors_InvalidPhysicalAddress = (1 << 5),
    MemoryAllocationErrors_Unknown = (1 << 6)
} MemoryAllocationErrors;

typedef struct MemoryAllocationsMap
{
    CachingMode 		CacheMode;
    uint64_t	 	VirtualAddress;
    uint64_t 		Length;
    MemoryAllocationFlags Flags;
    MemoryAllocationType 	AllocationType;
    uint64_t 		AdditionalData;
    uint64_t 		ReferenceCount;

    struct MemoryAllocationsMap *next;
} MemoryAllocationsMap;

void*
GetVirtualAddress(CachingMode 	c,
                  void 		*physicalAddress);

void*
GetPhysicalAddress(void *virtualAddress);

MemoryAllocationErrors
CreateVirtualMemoryInstance(UID *inst);

UID
SetActiveVirtualMemoryInstance(UID inst);

UID
GetActiveVirtualMemoryInstance(void);

MemoryAllocationErrors
MapPage(UID 			pageTable,
        MemoryAllocationsMap	*allocationMap,
        uint64_t 		physicalAddress,
        uint64_t 		virtualAddress,
        size_t			size,
        CachingMode 		cacheMode,
        MemoryAllocationType 	allocType,
        MemoryAllocationFlags 	flags);

MemoryAllocationErrors
UnmapPage(UID 			pageTable,
          MemoryAllocationsMap *allocationMap,
          uint64_t 		virtualAddress,
          size_t 		size);

MemoryAllocationErrors
FindFreeVirtualAddress(UID 			pageTable,
                       uint64_t			*virtualAddress,
                       size_t 			size,
                       MemoryAllocationType 	UNUSED(allocType),
                       MemoryAllocationFlags 	flags);

uint64_t
AllocatePhysicalPage(void);

uint64_t
AllocatePhysicalPageCont(int pageCount);

void
FreePhysicalPageCont(uint64_t ptr,
                     int pageCount);

#endif
