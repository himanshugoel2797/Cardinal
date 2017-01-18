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

#define REGISTER(x) RegisterSyscall(Syscall_##x, (SyscallHandler)##x_Syscall)

void
RegisterAllSyscalls(void) {

    MemoryInitLocks();
    PropertyInitLocks();

    REGISTER(Nanosleep);
    REGISTER(Brk);

    REGISTER(GetIPCMessageFrom);
    REGISTER(PostIPCMessage);

    REGISTER(SetProperty);
    REGISTER(GetProperty);

    REGISTER(GetErrno);

    REGISTER(R0_Map);
    REGISTER(R0_Unmap);

    REGISTER(Unmap);

    REGISTER(R0_AllocatePages);
    REGISTER(R0_FreePages);

    REGISTER(R0_CreateProcess);
    REGISTER(R0_StartProcess);
    REGISTER(R0_CreateThread);
    REGISTER(R0_KillProcess);
    REGISTER(R0_GetBootInfo);

    REGISTER(R01_GetPhysicalAddress);

    REGISTER(R0_AllocateSharedMemory);

    REGISTER(AllocateSharedMemory);
    REGISTER(GetSharedMemoryKey);
    REGISTER(ApplySharedMemoryKey);
    REGISTER(FreeSharedMemoryKey);
    REGISTER(GetSharedMemoryKeyUsageCount);

    REGISTER(WaitForMessage);
    REGISTER(GetIPCMessageMsgType);

    REGISTER(R01_RegisterForInterrupts);
    REGISTER(R01_AllocateInterrupts);
}