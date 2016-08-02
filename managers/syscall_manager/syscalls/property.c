#include "syscalls_all.h"
#include "syscalls/arch_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/cardinal_property.h"

#include "libs/libc/include/thread.h"
#include "libs/libc/include/asm/prctl.h"

#include "managers.h"

uint64_t
SetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_SetProperty)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 3)
        return EINVAL;

    switch(data->params[0]) {
    case CardinalProperty_SetTidAddress:
        return set_tid_address((void*)data->params[2]);
        break;
    case CardinalProperty_RegisterIPCMessageHandler:
        return RegisterMessageHandler((void (*)(Message*))data->params[2]);
        break;
#ifdef x86_64
    case CardinalProperty_ArchPrctl:
        return ArchPrctl_Syscall(data->params[1], data->params[2]);
        break;
    case CardinalProperty_IOPL:
        return SecurityMonitor_IOPL(data->params[2]);
        break;
#endif
    default:
        return EINVAL;
        break;
    }
}

uint64_t
GetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetProperty)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2)
        return EINVAL;

    switch(data->params[0]) {
    default:
        return EINVAL;
        break;
    }

}