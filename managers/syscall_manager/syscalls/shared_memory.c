#include "memory.h"
#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/shared_memory.h"

#include "managers.h"


uint64_t
AllocateSharedMemory_Syscall(uint64_t UNUSED(instruction_pointer),
                             uint64_t syscall_num,
                             uint64_t *syscall_params) {
    if(syscall_num != Syscall_AllocateSharedMemory) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 4) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    uint64_t length = data->params[0];

    if(length % PAGE_SIZE)
        length += PAGE_SIZE - length % PAGE_SIZE;

    CachingMode cacheMode = data->params[1];
    MemoryAllocationType allocType = MemoryAllocationType_MMap;
    MemoryAllocationFlags flags = data->params[3];

    flags &= ~MemoryAllocationFlags_Kernel;
    flags |= MemoryAllocationFlags_User;

    uint64_t vAddress = 0;
    MemoryAllocationErrors err = AllocateSharedMemory(GetCurrentProcessUID(),
                                 length,
                                 cacheMode,
                                 allocType,
                                 flags,
                                 &vAddress);

    if(err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return vAddress;
}

uint64_t
R0AllocateSharedMemory_Syscall(uint64_t UNUSED(instruction_pointer),
                               uint64_t syscall_num,
                               uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_AllocateSharedMemory) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 5) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    uint64_t length = data->params[0];

    if(length % PAGE_SIZE)
        length += PAGE_SIZE - length % PAGE_SIZE;

    CachingMode cacheMode = data->params[1];
    MemoryAllocationType allocType = data->params[2];
    MemoryAllocationFlags flags = data->params[3];
    uint64_t phys_addr = data->params[4];

    flags &= ~MemoryAllocationFlags_Kernel;
    flags |= MemoryAllocationFlags_User;

    uint64_t vAddress = 0;
    MemoryAllocationErrors err = AllocateSharedMemoryPhys(GetCurrentProcessUID(),
                                 length,
                                 cacheMode,
                                 allocType,
                                 flags,
                                 phys_addr,
                                 &vAddress);

    if(err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return vAddress;
}

uint64_t
GetSharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
                           uint64_t syscall_num,
                           uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetSharedMemoryKey) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 4) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    uint64_t vAddress = data->params[0];
    uint64_t length = data->params[1];
    if(length % PAGE_SIZE)
        length += PAGE_SIZE - length % PAGE_SIZE;

    CachingMode cacheMode = data->params[2];
    MemoryAllocationFlags flags = data->params[3];

    flags &= ~MemoryAllocationFlags_Kernel;
    flags |= MemoryAllocationFlags_User;

    uint64_t key = 0;
    MemoryAllocationErrors err = GetSharedMemoryKey(GetCurrentProcessUID(),
                                 vAddress,
                                 length,
                                 cacheMode,
                                 flags,
                                 &key);

    if(err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return key;
}

uint64_t
GetSharedMemoryKeyUsageCount_Syscall(uint64_t UNUSED(instruction_pointer),
                                     uint64_t syscall_num,
                                     uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetSharedMemoryKeyUsageCount) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    uint64_t key = data->params[0];
    uint64_t cnt = 0;

    MemoryAllocationErrors err = GetSharedMemoryKeyUsageCount(key, &cnt);

    if(err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return cnt;
}

uint64_t
ApplySharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
                             uint64_t syscall_num,
                             uint64_t *syscall_params) {
    if(syscall_num != Syscall_ApplySharedMemoryKey) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    uint64_t vAddress = 0;
    uint64_t length = 0;
    MemoryAllocationFlags flags = 0;
    CachingMode cacheMode = 0;

    MemoryAllocationErrors err = ApplySharedMemoryKey(GetCurrentProcessUID(),
                                 data->params[0],
                                 &vAddress,
                                 &flags,
                                 &cacheMode,
                                 &length);

    if(err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //TODO check the address to make sure it's safe!
    UserSharedMemoryData *shmem_data = (UserSharedMemoryData*)data->params[1];
    shmem_data->VirtualAddress = (void*)vAddress;
    shmem_data->Length = length;
    shmem_data->Flags = flags;
    shmem_data->CacheMode = cacheMode;

    return SyscallSetErrno(0);
}

uint64_t
FreeSharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
                            uint64_t syscall_num,
                            uint64_t *syscall_params) {
    if(syscall_num != Syscall_FreeSharedMemoryKey) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    MemoryAllocationErrors err = FreeSharedMemoryKey(GetCurrentProcessUID(),
                                 data->params[0]);

    if(err != MemoryAllocationErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    return SyscallSetErrno(0);
}