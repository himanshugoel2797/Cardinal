#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libc/include/thread.h"

#include "managers.h"

uint64_t
set_tid_address(void* address) {
    SetClearChildTIDAddress(GetCurrentThreadUID(),
                            address);

    return GetCurrentThreadUID();
}

uint64_t
R0CreateProcess_Syscall(uint64_t UNUSED(instruction_pointer),
                uint64_t syscall_num,
                uint64_t *syscall_params) 
{
	if(syscall_num != Syscall_R0_CreateProcess){
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0){
        SyscallSetErrno(-EPERM);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    UID pid = 0;
    switch(CreateProcess(data->params[0], data->params[1], &pid)){
    	case ProcessErrors_None:
    		SyscallSetErrno(0);
    		return pid;
    	break;
    	case ProcessErrors_Unknown:
    		SyscallSetErrno(-EUNKNWN);
    		return 0;
    	break;
    	case ProcessErrors_UIDNotFound:
    	case ProcessErrors_InvalidParameters:
    		SyscallSetErrno(-EINVAL);
    		return 0;
    	break;
    }

    return 0;
}

uint64_t
R0StartProcess_Syscall(uint64_t UNUSED(instruction_pointer),
                uint64_t syscall_num,
                uint64_t *syscall_params)
{
	if(syscall_num != Syscall_R0_StartProcess){
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0){
        SyscallSetErrno(-EPERM);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    switch(StartProcess(data->params[0])){
    	case ProcessErrors_None:
    		SyscallSetErrno(0);
    		return 0;
    	break;
    	case ProcessErrors_Unknown:
    		SyscallSetErrno(-EUNKNWN);
    		return -1;
    	break;
    	case ProcessErrors_UIDNotFound:
    	case ProcessErrors_InvalidParameters:
    		SyscallSetErrno(-EINVAL);
    		return -1;
    	break;
    }

    //Execution should never reach here.
    return -1;
}