#include "syscalls_all.h"
#include "priv_syscalls.h"

#include "boot_information/boot_information.h"
#include "common/common.h"

#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/syscall_property.h"

#include "libs/libc/include/thread.h"

#include "managers.h"

uint64_t
R0GetBootInfo_Syscall(uint64_t UNUSED(instruction_pointer),
                      uint64_t syscall_num,
                      uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_GetBootInfo) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }


    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    //TODO ensure this address is valid
    CardinalBootInfo *info = GetBootInfo();
    memcpy((CardinalBootInfo*)data->params[0], info, sizeof(CardinalBootInfo));

    return SyscallSetErrno(0);
}