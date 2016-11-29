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
    if(syscall_num != Syscall_GetIPCMessageFrom) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;
    if(data->param_num != 3) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //Check bottom end of buffer
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          data->params[0],
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //Check top end of buffer
    cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          data->params[0] + MESSAGE_SIZE,
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

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
    if(syscall_num != Syscall_PostIPCMessage) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 3) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //Check bottom end of buffer
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          data->params[1],
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //Check top end of buffer
    cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          data->params[1] + MESSAGE_SIZE * data->params[2],
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint64_t retVal = PostMessages(data->params[0], (Message**)data->params[1], data->params[2]);
    switch(retVal) {
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


uint64_t
GetResponseKey_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params) {
    if(syscall_num != Syscall_GetResponseKey) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint64_t retVal = CreateResponseBufferKey(GetCurrentProcessUID(), (uint32_t)data->params[0], (uint32_t)data->params[1]);
    switch(retVal) {
    case 0:
        SyscallSetErrno(-ENOMEM);
        return -1;
        break;
    default:
        SyscallSetErrno(0);
        return retVal;
        break;
    }
}

uint64_t
SubmitResponse_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params) {
    if(syscall_num != Syscall_SubmitResponse) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 3) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    if(data->params[1] == 0 | data->params[2] == 0) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //Check bottom end of buffer
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          data->params[1],
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    //Check top end of buffer
    cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          data->params[1] + data->params[2],
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    ProcessErrors err = SubmitToResponseBuffer(data->params[0], (void*)data->params[1], data->params[2]);

    switch(err) {
    case ProcessErrors_InvalidParameters:
    case ProcessErrors_UIDNotFound:
        SyscallSetErrno(-EINVAL);
        return -1;
        break;
    case ProcessErrors_Unknown:
        SyscallSetErrno(-EUNKNWN);
        return -1;
        break;
    default:
        return SyscallSetErrno(0);
        break;
    }
}


uint64_t
QueryResponseKeyLength_Syscall(uint64_t UNUSED(instruction_pointer),
                               uint64_t syscall_num,
                               uint64_t *syscall_params) {
    if(syscall_num != Syscall_QueryResponseKeyLength) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint32_t len = 0;
    ProcessErrors err = QueryResponseKeyLength(data->params[0], &len);

    switch(err) {
    case ProcessErrors_InvalidParameters:
    case ProcessErrors_UIDNotFound:
        SyscallSetErrno(-EINVAL);
        return -1;
        break;
    case ProcessErrors_Unknown:
        SyscallSetErrno(-EUNKNWN);
        return -1;
        break;
    default:
        SyscallSetErrno(0);
        return len;
        break;
    }

}