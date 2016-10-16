#include "syscalls_all.h"
#include "priv_syscalls.h"

#include "syscalls/arch_syscalls.h"
#include "boot_information/boot_information.h"
#include "common/common.h"

#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/syscall_property.h"

#include "libs/libc/include/thread.h"
#include "libs/libc/include/asm/prctl.h"

#include "managers.h"

uint64_t
SetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_SetProperty){
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;



    if(data->param_num != 3){
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    //TODO interpret error codes
    switch(data->params[0]) {
    case CardinalProperty_SetTidAddress:
        return set_tid_address((void*)data->params[2]);
        break;
    case CardinalProperty_SpecialDestination:
        return (uint64_t)SetSpecialDestinationPID(data->params[1]) - 1;
        break;
    case CardinalProperty_Exit:
        TerminateProcess(GetCurrentProcessUID(), data->params[1]);
        YieldThread();
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
        SyscallSetErrno(-EINVAL);
        return 0;
        break;
    }
}

uint64_t
GetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetProperty){
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2){
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    switch(data->params[0]) {
    case CardinalProperty_PID:
        SyscallSetErrno(0);
        return GetCurrentProcessUID();
        break;
    case CardinalProperty_TID:
        SyscallSetErrno(0);
        return GetCurrentThreadUID();
        break;
    case CardinalProperty_GroupID: {
        uint64_t rVal = GetProcessGroupID(GetCurrentProcessUID());
        if(rVal == (uint64_t)-1)
        {
            SyscallSetErrno(-EPERM);
            return 0;
        }
        else {

            SyscallSetErrno(0);
            return rVal;
        }
    }
    break;
    case CardinalProperty_R0_BootInfo: {
        if(GetProcessGroupID(GetCurrentProcessUID()) != 0){
            SyscallSetErrno(-EPERM);
            return 0;
        }

        //TODO ensure this address is valid
        CardinalBootInfo *info = GetBootInfo();
        memcpy((CardinalBootInfo*)data->params[1], info, sizeof(CardinalBootInfo));

        return SyscallSetErrno(0);
    }
    break;
    case CardinalProperty_R0_PhysicalAddress: {
        if(GetProcessGroupID(GetCurrentProcessUID()) != 0){
            SyscallSetErrno(-EPERM);
            return 0;
        }

        SyscallSetErrno(0);
        return (uint64_t)GetPhysicalAddress((void*)(uint64_t*)data->params[1]);
    }
    break;
    default:
        SyscallSetErrno(-EINVAL);
        return 0;
        break;
    }

}