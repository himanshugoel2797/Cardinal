#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libc/include/signal.h"
#include "managers.h"

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

    sigset_t tmp_copy;

    if(len != sizeof(sigset_t))
        return EINVAL;

    GetProcessSigmask(GetCurrentProcessUID(), &tmp_copy);

    if(oldset != NULL) {
        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)oldset, NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return EINVAL;

        GetProcessSigmask(GetCurrentProcessUID(), oldset);
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
RTSigAction_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {

}