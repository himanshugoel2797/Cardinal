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

static Spinlock set_prop_lock, get_prop_lock;

void
PropertyInitLocks(void){
    set_prop_lock = CreateSpinlock();
    get_prop_lock = CreateSpinlock();
}

uint64_t
SetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_SetProperty) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;



    if(data->param_num != 3) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    LockSpinlock(set_prop_lock);

    switch(data->params[0]) {
    case CardinalProperty_SetTidAddress:
        {
            uint64_t retVal = set_tid_address((void*)data->params[2]);
            SyscallSetErrno(0);
            UnlockSpinlock(set_prop_lock);
            return retVal;
        }
        break;
    case CardinalProperty_SpecialDestination:
        {
            if(SetSpecialDestinationPID(data->params[1]))
            {
                SyscallSetErrno(0);
                UnlockSpinlock(set_prop_lock);
                return 1;
            }else{
                SyscallSetErrno(-ENAVAIL);
                UnlockSpinlock(set_prop_lock);
                return 0;
            }
        }
        break;
    case CardinalProperty_Exit:
        {
            UnlockSpinlock(set_prop_lock);
            TerminateProcess(GetCurrentProcessUID(), data->params[1]);
            YieldThread();
            return 0;
        }
        break;
    case CardinalProperty_GroupID:
        {
            uint64_t retVal = SetProcessGroupID(GetCurrentProcessUID(), data->params[2]);
            UnlockSpinlock(set_prop_lock);
            switch(retVal){
                case -1:
                    SyscallSetErrno(-EINVAL);
                    return 0;
                    break;
                case -2:
                    SyscallSetErrno(-EPERM);
                    return 0;
                    break;
                default:
                    SyscallSetErrno(0);
                    return retVal;
            }
        }
        break;
#ifdef x86_64
    case CardinalProperty_ArchPrctl:
        {
            uint64_t retVal = ArchPrctl_Syscall(data->params[1], data->params[2]);
            UnlockSpinlock(set_prop_lock);
            return retVal;
        }
        break;
    case CardinalProperty_IOPL:
        {
            if(GetProcessGroupID(GetCurrentProcessUID()) > 1) {
                SyscallSetErrno(-EPERM);
                UnlockSpinlock(set_prop_lock);
                return 0;
            }

            SecurityMonitor_IOPL(data->params[2]);
            UnlockSpinlock(set_prop_lock);
            return SyscallSetErrno(0);
        }
        break;
#endif
    default:
        SyscallSetErrno(-EINVAL);
        UnlockSpinlock(set_prop_lock);
        return 0;
        break;
    }
}

uint64_t
GetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetProperty) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    LockSpinlock(get_prop_lock);

    switch(data->params[0]) {
    case CardinalProperty_PID:
        {
            SyscallSetErrno(0);
            uint64_t retVal = GetCurrentProcessUID();
            UnlockSpinlock(get_prop_lock);
            return retVal;
        }
        break;
    case CardinalProperty_TID:
        {
            SyscallSetErrno(0);
            uint64_t retVal = GetCurrentThreadUID();
            UnlockSpinlock(get_prop_lock);
            return retVal;
        }
        break;
    case CardinalProperty_GroupID: 
    {
        uint64_t rVal = GetProcessGroupID(GetCurrentProcessUID());
        if(rVal == (uint64_t)-1) {
            SyscallSetErrno(-EPERM);
            UnlockSpinlock(get_prop_lock);
            return 0;
        } else {
            SyscallSetErrno(0);
            UnlockSpinlock(get_prop_lock);
            return rVal;
        }
    }
    break;
    case CardinalProperty_R0_BootInfo: {
        if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
            SyscallSetErrno(-EPERM);
            UnlockSpinlock(get_prop_lock);
            return 0;
        }

        //TODO ensure this address is valid
        CardinalBootInfo *info = GetBootInfo();
        memcpy((CardinalBootInfo*)data->params[1], info, sizeof(CardinalBootInfo));

        UnlockSpinlock(get_prop_lock);
        return SyscallSetErrno(0);
    }
    break;
    case CardinalProperty_R0_PhysicalAddress: {
        if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
            SyscallSetErrno(-EPERM);
            UnlockSpinlock(get_prop_lock);
            return 0;
        }

        SyscallSetErrno(0);
        uint64_t retVal = (uint64_t)GetPhysicalAddress((void*)(uint64_t*)data->params[1]);
        UnlockSpinlock(get_prop_lock);
        return retVal;
    }
    break;
    default:
        SyscallSetErrno(-EINVAL);
        UnlockSpinlock(get_prop_lock);
        return 0;
        break;
    }

}