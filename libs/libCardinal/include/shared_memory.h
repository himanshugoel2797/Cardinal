#ifndef _CARDINAL_SHMEM_H_
#define _CARDINAL_SHMEM_H_

#include "cardinal_types.h"
#include "memory.h"

typedef struct {
	void *VirtualAddress;
	uint64_t Length;
	MemoryAllocationFlags Flags;
	CachingMode CacheMode;
} UserSharedMemoryData;

#endif