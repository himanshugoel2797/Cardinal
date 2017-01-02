/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libc/include/thread.h"

#include "managers.h"

uint64_t
R0CreateProcess_Syscall(uint64_t UNUSED(instruction_pointer),
                        uint64_t syscall_num,
                        uint64_t *syscall_params) {

    if(syscall_num != Syscall_R0_CreateProcess) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    UID pid = 0;
    switch(CreateProcess(data->params[0], data->params[1], &pid)) {
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
    case ProcessErrors_OutOfMemory:
        SyscallSetErrno(-EINVAL);
        return 0;
        break;
    }

    return 0;
}

uint64_t
R0CreateThread_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_CreateThread) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 3) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    UID pid = CreateThread(data->params[0], ThreadPermissionLevel_User, (ThreadEntryPoint)data->params[1], (void*)data->params[2]);
    switch(ProcessErrors_None) {
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
                       uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_StartProcess) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    switch(StartProcess(data->params[0])) {
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
    case ProcessErrors_OutOfMemory:
        SyscallSetErrno(-EINVAL);
        return -1;
        break;
    }

    //Execution should never reach here.
    return -1;
}


uint64_t
WaitForMessage_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params) {

    if(syscall_num != Syscall_WaitForMessage) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SleepThreadForMessage(GetCurrentThreadUID(), data->params[0], data->params[1]);

    SyscallSetErrno(0);
    return 0;
}


uint64_t
R0KillProcess_Syscall(uint64_t UNUSED(instruction_pointer),
                      uint64_t syscall_num,
                      uint64_t *syscall_params) {

    if(syscall_num != Syscall_R0_KillProcess) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    uint64_t rVal = GetProcessGroupID(GetCurrentProcessUID());
    if(rVal != 0) {
        SyscallSetErrno(-EPERM);
        return -1;
    }

    ScheduleProcessForTermination(data->params[0], data->params[1]);

    SyscallSetErrno(0);
    return 0;
}