#ifndef _CARDINAL_SHMEM_H_
#define _CARDINAL_SHMEM_H_

#include "syscall.h"
#include "syscall_list.h"
#include "cardinal_types.h"
#include "memory.h"

typedef struct {
	void *VirtualAddress;
	uint64_t Length;
	MemoryAllocationFlags Flags;
	CachingMode CacheMode;
} UserSharedMemoryData;

#ifndef _KERNEL_

static __inline uint64_t
AllocateSharedMemory(uint64_t length,
					 CachingMode cacheMode,
					 MemoryAllocationType allocType,
					 MemoryAllocationFlags flags,
					 uint64_t *virtualAddress) {
	if(virtualAddress == NULL)
		return -EINVAL;

	*virtualAddress = Syscall4(Syscall_AllocateSharedMemory, length, cacheMode, allocType, flags);
	return GetErrno();
}

static __inline uint64_t
R0_AllocateSharedMemory(uint64_t length,
					 CachingMode cacheMode,
					 MemoryAllocationType allocType,
					 MemoryAllocationFlags flags,
					 uint64_t physicalAddress,
					 uint64_t *virtualAddress) {
	if(virtualAddress == NULL)
		return -EINVAL;

	*virtualAddress = Syscall5(Syscall_AllocateSharedMemory, length, cacheMode, allocType, flags, physicalAddress);
	return GetErrno();
}

static __inline uint64_t
GetSharedMemoryKey(uint64_t virtualAddress,
				   uint64_t length,
				   CachingMode cacheMode,
				   MemoryAllocationFlags flags,
				   uint64_t *key) {
	if(key == NULL)
		return -EINVAL;

	*key = Syscall4(Syscall_GetSharedMemoryKey, virtualAddress, length, cacheMode, flags);
	return GetErrno();
}

static __inline uint64_t
ApplySharedMemoryKey(uint64_t key,
					 UserSharedMemoryData *data) {
	if(data == NULL)
		return -EINVAL;

	Syscall2(Syscall_ApplySharedMemoryKey, key, (uint64_t)data);
	return GetErrno();
}

static __inline uint64_t
FreeSharedMemoryKey(uint64_t key) {
	Syscall1(Syscall_FreeSharedMemoryKey, key);
	return GetErrno();
}

#endif

#endif