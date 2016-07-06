#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"

#include "time.h"

uint64_t
Nanosleep_Syscall(uint64_t UNUSED(instruction_pointer),
                  uint64_t syscall_num,
                  uint64_t *syscall_params) {
    SyscallData *data = (SyscallData*)syscall_params;

    if(syscall_num != Syscall_Nanosleep)
        return ENOSYS;

    if(data->param_num != 2)
        return EINVAL;

    //SleepThread(GetCurrentThreadUID(), )
    return 0;
}