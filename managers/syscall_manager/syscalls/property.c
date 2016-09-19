#include "syscalls_all.h"
#include "syscalls/arch_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/syscall_property.h"

#include "libs/libc/include/thread.h"
#include "libs/libc/include/asm/prctl.h"

#include "managers.h"

uint64_t
SetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_SetProperty)
        return -ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;



    if(data->param_num != 3)
        return -EINVAL;

    switch(data->params[0]) {
    case CardinalProperty_SetTidAddress:
        return set_tid_address((void*)data->params[2]);
        break;
    case CardinalProperty_SpecialDestination:
        return (uint64_t)SetSpecialDestinationPID(data->params[1]) - 1;
        break;
    case CardinalProperty_Exit:
        __asm__ volatile("cli\n\thlt" :: "a"(data->params[1]));
        TerminateProcess(GetCurrentProcessUID(), data->params[1]);
        return 0;
        break;
    case CardinalProperty_GroupID:
        return (uint64_t)SetProcessGroupID(GetCurrentProcessUID(), data->params[2]);
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
        return -EINVAL;
        break;
    }
}

uint64_t
GetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetProperty)
        return -ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2)
        return -EINVAL;

    switch(data->params[0]) {
    case CardinalProperty_PID:
        return GetCurrentProcessUID();
        break;
    case CardinalProperty_TID:
        return GetCurrentThreadUID();
        break;
    case CardinalProperty_GroupID:
        { 
            uint64_t rVal = GetProcessGroupID(GetCurrentProcessUID());
            if(rVal == (uint64_t)-1)return -EPERM;
            else return rVal;
        }
        break;
    case CardinalProperty_PLS:
        return (uint64_t)GetProcessLocalStorage(GetCurrentProcessUID(), syscall_params[1]);
        break;
    default:
        return -EINVAL;
        break;
    }

}