// Copyright (c) 2017 Himanshu Goel
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _CARDINAL_SHMEM_H_
#define _CARDINAL_SHMEM_H_

#include "syscall.h"
#include "syscall_list.h"
#include "cardinal_types.h"
#include "memory.h"

#define KEY_LEN (512 / 8)

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

    *virtualAddress = Syscall5(Syscall_R0_AllocateSharedMemory, length, cacheMode, allocType, flags, physicalAddress);
    return GetErrno();
}

static __inline uint64_t
GetSharedMemoryKey(uint64_t virtualAddress,
                   uint64_t length,
                   CachingMode cacheMode,
                   MemoryAllocationFlags flags,
                   uint8_t key[KEY_LEN]) {
    if(key == NULL)
        return -EINVAL;

    Syscall5(Syscall_GetSharedMemoryKey, virtualAddress, length, cacheMode, flags, key);
    return GetErrno();
}

static __inline uint64_t
GetSharedMemoryKeyUsageCount(uint128_t key[KEY_LEN],
                             uint64_t *cnt) {
    if(cnt == NULL)
        return -EINVAL;

    *cnt = Syscall1(Syscall_GetSharedMemoryKeyUsageCount, key);
    return GetErrno();
}

static __inline uint64_t
ApplySharedMemoryKey(uint8_t key[KEY_LEN],
                     UserSharedMemoryData *data) {
    if(data == NULL)
        return -EINVAL;

    Syscall2(Syscall_ApplySharedMemoryKey, key, (uint64_t)data);
    return GetErrno();
}

static __inline uint64_t
FreeSharedMemoryKey(uint8_t key[KEY_LEN]) {
    Syscall1(Syscall_FreeSharedMemoryKey, key);
    return GetErrno();
}

#endif

#endif