#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "managers.h"
#include "common.h"
#include "kmalloc.h"
#include "synchronization.h"

uint64_t
R01AllocateInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                              uint64_t syscall_num,
                              uint64_t *syscall_params) {
    if(syscall_num != Syscall_R01_AllocateInterrupts) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) > 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;
    if(data->param_num != 1) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    int retVal = InterruptMan_AllocateBlock((int)data->params[0]);
    if(retVal == -1) {
        SyscallSetErrno(-ENOMEM);
        return (uint64_t)retVal;
    }

    SyscallSetErrno(0);
    return retVal;
}

uint64_t
R01RegisterForInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                                 uint64_t syscall_num,
                                 uint64_t *syscall_params) {
    if(syscall_num != Syscall_R01_RegisterForInterrupts) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) > 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;
    if(data->param_num != 2) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    int result = InterruptMan_RegisterProcess(GetCurrentProcessUID(),
                 data->params[0],
                 data->params[1]);

    if(result != 0) {
        SyscallSetErrno(-EUNKNWN);
        return (uint64_t)result;
    }

    SyscallSetErrno(0);
    return (uint64_t)result;
}
