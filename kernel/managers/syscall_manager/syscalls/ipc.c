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
#include "libs/libCardinal/include/file_server.h"
#include "managers.h"
#include "common.h"
#include "kmalloc.h"
#include "synchronization.h"

uint64_t
GetIPCMessageFrom_Syscall(Message *p0,
                          UID p1,
                          uint64_t p2) {
    //Check bottom end of buffer
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          (uint64_t)p0,
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
                          (uint64_t)p0 + MESSAGE_SIZE,
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint64_t retVal = GetMessageFrom(p0, p1, p2);
    if(retVal == 0)
        SyscallSetErrno(-ENOMSG);
    else
        SyscallSetErrno(0);

    return retVal;
}

uint64_t
GetIPCMessageMsgType_Syscall(Message *p0,
                             UID p1) {
    //Check bottom end of buffer
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          (uint64_t)p0,
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
                          (uint64_t)p0 + MESSAGE_SIZE,
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint64_t retVal = GetMessageFromType(p0, p1);
    if(retVal == 0)
        SyscallSetErrno(-ENOMSG);
    else
        SyscallSetErrno(0);

    return retVal;
}

uint64_t
PostIPCMessage_Syscall(uint64_t p0,
                       Message **p1,
                       uint64_t p2) {

    //Check bottom end of buffer
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          (uint64_t)p1,
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
                          (uint64_t)p1 + MESSAGE_SIZE * p2,
                          NULL,
                          &cFlags,
                          NULL);

    if(cFlags != (MemoryAllocationFlags_User | MemoryAllocationFlags_Present | MemoryAllocationFlags_Write)) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint64_t retVal = PostMessages(p0, p1, p2);
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