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
    uint64_t retVal = GetMessageFrom((Message*)data->params[0], (UID)data->params[1], data->params[2]);
    if(retVal == 0)
      SyscallSetErrno(-ENOMSG);
    else
      SyscallSetErrno(0);

    return retVal;
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
    
    if(data->param_num != 3){
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint64_t retVal = PostMessages(data->params[0], (Message**)data->params[1], data->params[2]);
    switch(retVal){
      case -1:
        SyscallSetErrno(-EPERM);
        return -1;
      break;
      case -2:
        SyscallSetErrno(-EINVAL);
        return -1;
      break;
      default:
        SyscallSetErrno(0);
        return retVal;
      break;
    }
}

