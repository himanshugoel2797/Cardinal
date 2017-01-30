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
R01_AllocateInterrupts_Syscall(int cnt) {

    if(GetProcessGroupID(GetCurrentProcessUID()) > 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    int retVal = InterruptMan_AllocateBlock(cnt);
    if(retVal == -1) {
        SyscallSetErrno(-ENOMEM);
        return (uint64_t)retVal;
    }

    SyscallSetErrno(0);
    return retVal;
}

uint64_t
R01_RegisterForInterrupts_Syscall(uint64_t p0,
                                  uint64_t p1) {
    if(GetProcessGroupID(GetCurrentProcessUID()) > 1) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    int result = InterruptMan_RegisterProcess(GetCurrentProcessUID(),
                 p0,
                 p1);

    if(result != 0) {
        SyscallSetErrno(-EUNKNWN);
        return (uint64_t)result;
    }

    SyscallSetErrno(0);
    return (uint64_t)result;
}
