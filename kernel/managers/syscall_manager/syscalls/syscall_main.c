/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "libs/libCardinal/include/syscall.h"
#include "priv_syscalls.h"
#include "syscall.h"
#include "syscalls_all.h"

#define REGISTER(x) RegisterSyscall(Syscall_##x, (SyscallHandler)x##_Syscall)

void RegisterAllSyscalls(void) {
    MemoryInitLocks();
    PropertyInitLocks();

    REGISTER(Nanosleep);
    REGISTER(R0_GetBootInfo);

    REGISTER(SetProperty);
    REGISTER(GetProperty);

    REGISTER(GetErrno);

    REGISTER(Brk);

    REGISTER(R01_GetPhysicalAddress);

    REGISTER(CreateProcess);

    REGISTER(CreateKey);
    REGISTER(AddKey);
    // REGISTER(Dup);
    REGISTER(RemoveKey);
    REGISTER(GetKeyIndex);
    REGISTER(UseKey);
    REGISTER(GetKeyUsageCount);
}