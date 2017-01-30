/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "syscalls_all.h"
#include "priv_syscalls.h"

#include "boot_information/boot_information.h"
#include "common/common.h"

#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/syscall_property.h"

#include "libs/libc/include/thread.h"

#include "managers.h"

uint64_t
R0_GetBootInfo_Syscall(int syscall_num,
                      void *copy_dst) {
    if(syscall_num != Syscall_R0_GetBootInfo) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    if(copy_dst == NULL){
        SyscallSetErrno(-EINVAL);
        return 0;
    }

    //TODO ensure this address is valid
    CardinalBootInfo *info = GetBootInfo();
    memcpy(copy_dst, info, sizeof(CardinalBootInfo));

    return SyscallSetErrno(0);
}