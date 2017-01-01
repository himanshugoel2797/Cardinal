/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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

    *virtualAddress = Syscall5(Syscall_R0_AllocateSharedMemory, length, cacheMode, allocType, flags, physicalAddress);
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
GetSharedMemoryKeyUsageCount(uint64_t key,
                             uint64_t *cnt) {
    if(cnt == NULL)
        return -EINVAL;

    *cnt = Syscall1(Syscall_GetSharedMemoryKeyUsageCount, key);
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