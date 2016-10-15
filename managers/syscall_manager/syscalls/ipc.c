#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/file_server.h"
#include "managers.h"
#include "common.h"
#include "kmalloc.h"
#include "synchronization.h"

uint64_t
GetIPCMessageFrom_Syscall(uint64_t UNUSED(instruction_pointer),
                          uint64_t syscall_num,
                          uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetIPCMessageFrom){
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;
    if(data->param_num != 3){
      SyscallSetErrno(-EINVAL);
      return -1;
    }

    //TODO interpret returned error codes
    return GetMessageFrom((Message*)data->params[0], (UID)data->params[1], data->params[2]);
}

uint64_t
PostIPCMessage_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params) {
    if(syscall_num != Syscall_PostIPCMessage){
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;
    
    if(data->param_num != 2){
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //TODO interpret returned error codes
    return PostMessages((Message**)data->params[0], data->params[1]);
}

