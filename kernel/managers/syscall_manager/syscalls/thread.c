/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libc/include/thread.h"

#include "managers.h"

uint64_t R0_CreateProcess_Syscall(UID parent, uint64_t gid) {
    if (GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    UID pid = 0;
    switch (CreateProcess(parent, gid, &pid)) {
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

uint64_t R0_CreateThread_Syscall(UID parent,
                                 ThreadEntryPoint entry_point,
                                 void* arg) {
    if (GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    UID pid = CreateThread(parent, ThreadPermissionLevel_User, entry_point, arg);
    switch (ProcessErrors_None) {
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

uint64_t R0_StartProcess_Syscall(UID pid) {
    if (GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return -1;
    }

    switch (StartProcess(pid)) {
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

    // Execution should never reach here.
    return -1;
}

uint64_t R0_KillProcess_Syscall(UID pid, uint32_t exit_code) {
    uint64_t rVal = GetProcessGroupID(GetCurrentProcessUID());
    if (rVal != 0) {
        SyscallSetErrno(-EPERM);
        return -1;
    }

    ScheduleProcessForTermination(pid, exit_code);

    SyscallSetErrno(0);
    return 0;
}