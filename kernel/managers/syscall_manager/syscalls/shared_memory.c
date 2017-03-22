/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "libs/libCardinal/include/shared_memory.h"
#include "libs/libCardinal/include/keyman.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "priv_syscalls.h"
#include "syscalls_all.h"

#include "managers.h"

uint64_t AllocateSharedMemory_Syscall(uint64_t length,
                                      CachingMode cacheMode,
                                      void* UNUSED(unused),
                                      MemoryAllocationFlags flags) {
    if (length % PAGE_SIZE)
        length += PAGE_SIZE - length % PAGE_SIZE;

    MemoryAllocationType allocType =
        MemoryAllocationType_MMap | MemoryAllocationType_Shared;

    flags &= ~MemoryAllocationFlags_Kernel;
    flags |= MemoryAllocationFlags_User;

    uint64_t vAddress = 0;
    MemoryAllocationErrors err = AllocateSharedMemory(
                                     GetCurrentProcessUID(), length, cacheMode, allocType, flags, &vAddress);

    if (err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return vAddress;
}

uint64_t R0_AllocateSharedMemory_Syscall(uint64_t length,
        CachingMode cacheMode,
        MemoryAllocationType allocType,
        MemoryAllocationFlags flags,
        uint64_t phys_addr) {
    if (GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if (length % PAGE_SIZE)
        length += PAGE_SIZE - length % PAGE_SIZE;

    allocType |=
        MemoryAllocationType_Shared | MemoryAllocationType_Phys;

    flags &= ~MemoryAllocationFlags_Kernel;
    flags |= MemoryAllocationFlags_User;

    uint64_t vAddress = 0;
    MemoryAllocationErrors err =
        AllocateSharedMemoryPhys(GetCurrentProcessUID(), length, cacheMode,
                                 allocType, flags, phys_addr, &vAddress);

    if (err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return vAddress;
}

uint64_t GetSharedMemoryKey_Syscall(uint64_t vAddress,
                                    uint64_t length,
                                    CachingMode cacheMode,
                                    MemoryAllocationFlags flags,
                                    Key_t *key) {
    if (length % PAGE_SIZE)
        length += PAGE_SIZE - length % PAGE_SIZE;

    flags &= ~MemoryAllocationFlags_Kernel;
    flags |= MemoryAllocationFlags_User;

    MemoryAllocationErrors err = GetSharedMemoryKey(
                                     GetCurrentProcessUID(), vAddress, length, cacheMode, flags, key);

    if (err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    return SyscallSetErrno(0);
}

uint64_t GetSharedMemoryKeyUsageCount_Syscall(Key_t *key) {
    uint64_t cnt = 0;

    MemoryAllocationErrors err = GetSharedMemoryKeyUsageCount(key, &cnt);

    if (err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return cnt;
}

uint64_t ApplySharedMemoryKey_Syscall(Key_t *key, void* shmem_data_p) {
    uint64_t vAddress = 0;
    uint64_t length = 0;
    MemoryAllocationFlags flags = 0;
    CachingMode cacheMode = 0;

    MemoryAllocationErrors err = ApplySharedMemoryKey(
                                     GetCurrentProcessUID(), key, &vAddress, &flags, &cacheMode, &length);

    if (err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    // TODO check the address to make sure it's safe!
    UserSharedMemoryData* shmem_data = (UserSharedMemoryData*)shmem_data_p;
    shmem_data->VirtualAddress = (void*)vAddress;
    shmem_data->Length = length;
    shmem_data->Flags = flags;
    shmem_data->CacheMode = cacheMode;

    return SyscallSetErrno(0);
}

uint64_t FreeSharedMemoryKey_Syscall(Key_t *key) {
    MemoryAllocationErrors err = FreeSharedMemoryKey(GetCurrentProcessUID(), key);

    if (err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    return SyscallSetErrno(0);
}