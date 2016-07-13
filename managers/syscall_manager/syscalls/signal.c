#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libc/include/signal.h"
#include "managers.h"
#include "common.h"

uint64_t
rt_sigprocmask(int how, const sigset_t* set, sigset_t* oldset, size_t len)
{
	    sigset_t tmp_copy;

    if(len != sizeof(sigset_t))
        return EINVAL;

    GetProcessSigmask(GetCurrentProcessUID(), &tmp_copy);

    if(oldset != NULL) {
        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)oldset, NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return EINVAL;

        memcpy(oldset, &tmp_copy, sizeof(sigset_t));
    }

    if(set != NULL) {

        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)set, NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return EINVAL;

        uint8_t *dst = (uint8_t*)&tmp_copy;
        uint8_t *src = (uint8_t*)set;

        for(uint64_t i = 0; i < sizeof(sigset_t); i++) {
            switch(how) {
            case SIG_BLOCK:
                dst[i] |= src[i];
                break;
            case SIG_UNBLOCK:
                dst[i] &= ~src[i];
                break;
            case SIG_SETMASK:
                dst[i] = src[i];
                break;
            default:
                return EINVAL;
            }
        }

        SetProcessSigmask(GetCurrentProcessUID(), &tmp_copy);
    }
    return 0;
}

uint64_t
RTSigProcMask_Syscall(uint64_t UNUSED(instruction_pointer),
                      uint64_t syscall_num,
                      uint64_t *syscall_params) {
    if(syscall_num != Syscall_RTSigProcMask)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 4)
        return ENOSYS;

    int how = data->params[0];
    const sigset_t* set = (const sigset_t*)data->params[1];
    sigset_t* oldset = (sigset_t*)data->params[2];
    size_t len = (size_t)data->params[3];


    return rt_sigprocmask(how, set, oldset, len);
}

uint64_t
rt_sigaction(int signum, 
             const sigaction *act,
             sigaction *oldact,
             size_t len)
{
    //TODO finish implementing sigaction
            sigaction tmp_copy;

    if(signum >= SUPPORTED_SIGNAL_COUNT)
        return EINVAL;

    if(len != sizeof(sigaction))
        return EINVAL;

    GetProcessSigaction(GetCurrentProcessUID(), signum, &tmp_copy);

    if(oldact != NULL) {
        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)oldact, NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return EINVAL;

        memcpy(oldact, &tmp_copy, sizeof(sigaction));
    }

    if(act != NULL) {

        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)act, NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return EINVAL;

        SetProcessSigaction(GetCurrentProcessUID(), signum, act);
    }
    return 0;
}

uint64_t
RTSigAction_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_RTSigAction)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;

	if(data->param_num != 4)
		return ENOSYS;

    int signum = data->params[0];
    const sigaction *act = (const sigaction*)data->params[1];
    sigaction *oldact = (sigaction*)data->params[2];
    size_t len = (size_t)data->params[3];

    return rt_sigaction(signum, act, oldact, len);
}