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

void
RegisterAllSyscalls(void) {

    MemoryInitLocks();
    PropertyInitLocks();

    RegisterSyscall(Syscall_Nanosleep, Nanosleep_Syscall);
    RegisterSyscall(Syscall_Brk, Brk_Syscall);

    RegisterSyscall(Syscall_GetIPCMessageFrom, GetIPCMessageFrom_Syscall);
    RegisterSyscall(Syscall_PostIPCMessage, PostIPCMessage_Syscall);

    RegisterSyscall(Syscall_SetProperty, SetProperty_Syscall);
    RegisterSyscall(Syscall_GetProperty, GetProperty_Syscall);

    RegisterSyscall(Syscall_GetErrno, GetErrno_Syscall);

    RegisterSyscall(Syscall_R0_Map, R0Map_Syscall);
    RegisterSyscall(Syscall_R0_Unmap, R0Unmap_Syscall);

    RegisterSyscall(Syscall_Unmap, Unmap_Syscall);

    RegisterSyscall(Syscall_R0_AllocatePages, R0AllocatePages_Syscall);
    RegisterSyscall(Syscall_R0_FreePages, R0FreePages_Syscall);

    RegisterSyscall(Syscall_R0_CreateProcess, R0CreateProcess_Syscall);
    RegisterSyscall(Syscall_R0_StartProcess, R0StartProcess_Syscall);
    RegisterSyscall(Syscall_R0_CreateThread, R0CreateThread_Syscall);
    RegisterSyscall(Syscall_R0_KillProcess, R0KillProcess_Syscall);
    RegisterSyscall(Syscall_R0_GetBootInfo, R0GetBootInfo_Syscall);

    RegisterSyscall(Syscall_R01_GetPhysicalAddress, R01GetPhysicalAddress_Syscall);

    RegisterSyscall(Syscall_R0_AllocateSharedMemory, R0AllocateSharedMemory_Syscall);

    RegisterSyscall(Syscall_AllocateSharedMemory, AllocateSharedMemory_Syscall);
    RegisterSyscall(Syscall_GetSharedMemoryKey, GetSharedMemoryKey_Syscall);
    RegisterSyscall(Syscall_ApplySharedMemoryKey, ApplySharedMemoryKey_Syscall);
    RegisterSyscall(Syscall_FreeSharedMemoryKey, FreeSharedMemoryKey_Syscall);
    RegisterSyscall(Syscall_GetSharedMemoryKeyUsageCount, GetSharedMemoryKeyUsageCount_Syscall);

    RegisterSyscall(Syscall_WaitForMessage, WaitForMessage_Syscall);
    RegisterSyscall(Syscall_GetIPCMessageMsgType, GetIPCMessageMsgType_Syscall);

    RegisterSyscall(Syscall_R01_RegisterForInterrupts, R01RegisterForInterrupts_Syscall);
    RegisterSyscall(Syscall_R01_AllocateInterrupts, R01AllocateInterrupts_Syscall);
}