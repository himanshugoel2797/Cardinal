#ifndef _CARDINAL_HAL_MEMORY_OPS_H_
#define _CARDINAL_HAL_MEMORY_OPS_H_

#include "types.h"

typedef enum
{
    CachingModeWriteBack = 0,
    CachingModeUncachable = 1,
    CachingModeWriteThrough = 2
} CachingMode;

typedef enum{
  MemoryAllocationType_Heap,
  MemoryAllocationType_Stack,
  MemoryAllocationType_MMap,
  MemoryAllocationType_Paged,
  MemoryAllocationType_Shared
}MemoryAllocationType;

typedef enum{
  MemoryAllocationFlags_NoExec,
  MemoryAllocationFlags_Exec,
  MemoryAllocationFlags_Write
}MemoryAllocationFlags;

typedef struct MemoryAllocationsMap{
  CachingMode CacheMode;
  uint64_t VirtualAddress;
  uint64_t Length;
  MemoryAllocationFlags Flags;
  MemoryAllocationType AllocationType;
  uint64_t AdditionalData;

  struct MemoryAllocationsMap *next;
}MemoryAllocationsMap;

void*
GetVirtualAddress(CachingMode 	c,
                  void 		*physicalAddress);

void*
GetPhysicalAddress(void *virtualAddress);

#endif
