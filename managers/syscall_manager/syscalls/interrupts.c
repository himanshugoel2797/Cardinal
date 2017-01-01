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
#include "managers.h"
#include "common.h"
#include "kmalloc.h"
#include "synchronization.h"

uint64_t
R01AllocateInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                              uint64_t syscall_num,
                              uint64_t *syscall_params) {
    if(syscall_num != Syscall_R01_AllocateInterrupts) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) > 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;
    if(data->param_num != 1) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    int retVal = InterruptMan_AllocateBlock((int)data->params[0]);
    if(retVal == -1) {
        SyscallSetErrno(-ENOMEM);
        return (uint64_t)retVal;
    }

    SyscallSetErrno(0);
    return retVal;
}

uint64_t
R01RegisterForInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                                 uint64_t syscall_num,
                                 uint64_t *syscall_params) {
    if(syscall_num != Syscall_R01_RegisterForInterrupts) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) > 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;
    if(data->param_num != 2) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    int result = InterruptMan_RegisterProcess(GetCurrentProcessUID(),
                 data->params[0],
                 data->params[1]);

    if(result != 0) {
        SyscallSetErrno(-EUNKNWN);
        return (uint64_t)result;
    }

    SyscallSetErrno(0);
    return (uint64_t)result;
}
