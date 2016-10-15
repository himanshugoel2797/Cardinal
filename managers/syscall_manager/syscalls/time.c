#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"

#include "time.h"

uint64_t
Nanosleep_Syscall(uint64_t UNUSED(instruction_pointer),
                  uint64_t syscall_num,
                  uint64_t *syscall_params) {
    SyscallData *data = (SyscallData*)syscall_params;

    if(syscall_num != Syscall_Nanosleep)
        return SyscallSetErrno(-ENOSYS);

    if(data->param_num != 1)
        return SyscallSetErrno(-EINVAL);

    SleepThread(GetCurrentThreadUID(), data->params[0]);

    return SyscallSetErrno(0);
}