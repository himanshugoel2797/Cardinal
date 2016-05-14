#ifndef _CARDINAL_HAL_MEMORY_OPS_H_
#define _CARDINAL_HAL_MEMORY_OPS_H_

#include "types.h"

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
#define PAGE_SIZE KiB(4)

#define THREAD_LOCAL_STORAGE_SIZE KiB(4)

///Memory Caching Modes
typedef enum {
    CachingModeWriteBack = 0,   ///<Write Back
    CachingModeUncachable = 1,  ///<Uncachable
    CachingModeWriteThrough = 2 ///<Write Through
} CachingMode;

///Memory Allocation Type
typedef enum {
    MemoryAllocationType_Heap = 0,          ///<Heap
    MemoryAllocationType_Stack = (1 << 0),  ///<Stack
    MemoryAllocationType_MMap = (1 << 1),   ///<Memory Mapped
    MemoryAllocationType_Fork = (1 << 2),   ///<Forked
    MemoryAllocationType_Paged = (1 << 3),  ///<Paged from Disk
    MemoryAllocationType_Shared = (1 << 4)  ///<Shared Memory
} MemoryAllocationType;

///Memory Allocation Flags
typedef enum {
    MemoryAllocationFlags_NoExec = (1 << 0),  ///<Execute Disable
    MemoryAllocationFlags_Exec = (1 << 1),    ///<Execute Enable
    MemoryAllocationFlags_Write = (1 << 2),   ///<Write Enabled
    MemoryAllocationFlags_Kernel = 0,         ///<Kernel Access Permission
    MemoryAllocationFlags_User = (1 << 3)     ///<User Access Permission
} MemoryAllocationFlags;

///Memory Allocation Errors
typedef enum {
    MemoryAllocationErrors_None = 0,                          ///<No Error
    MemoryAllocationErrors_AlreadyMapped = (1 << 0),          ///<Memory is already mapped
    MemoryAllocationErrors_NotMapped = (1 << 1),              ///<Memory is not mapped
    MemoryAllocationErrors_PartiallyMapped = (1 << 2),        ///<Memory is partially mapped
    MemoryAllocationErrors_InvalidFlags = (1 << 3),           ///<The Flags parameter is invalid
    MemoryAllocationErrors_InvalidVirtualAddress = (1 << 4),  ///<Invalid Virtual Address
    MemoryAllocationErrors_InvalidPhysicalAddress = (1 << 5), ///<Invalid Physical Address
    MemoryAllocationErrors_Unknown = (1 << 6)                 ///<Unknown Error
} MemoryAllocationErrors;

typedef struct MemoryAllocationsMap {
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

void*
GetPhysicalAddressUID(UID 	src,
                      void 	*virtualAddress);

MemoryAllocationErrors
CreateVirtualMemoryInstance(UID *inst);

UID
SetActiveVirtualMemoryInstance(UID inst);

UID
GetActiveVirtualMemoryInstance(void);

MemoryAllocationErrors
ForkTable(UID src,
          MemoryAllocationsMap *srcAllocBase,
          UID *dst,
          MemoryAllocationsMap **dstAllocBase);

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

///< Allocate some Application Processor Local Storage
void
AllocateAPLS(int coreID);

///< Retrieve the Application Processor Local Storage of a core
void*
GetAPLS(int coreID);

#endif
