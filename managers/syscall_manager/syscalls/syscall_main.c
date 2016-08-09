#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"

void
RegisterAllSyscalls(void) {
    RegisterSyscall(Syscall_Nanosleep, Nanosleep_Syscall);
    RegisterSyscall(Syscall_MMap, MMap_Syscall);
    RegisterSyscall(Syscall_Brk, Brk_Syscall);
    RegisterSyscall(Syscall_Clone, Clone_Syscall);
    RegisterSyscall(Syscall_Fork, Fork_Syscall);

    RegisterSyscall(Syscall_GetIPCMessageFrom, GetIPCMessageFrom_Syscall);
    RegisterSyscall(Syscall_PostIPCMessage, PostIPCMessage_Syscall);

    RegisterSyscall(Syscall_SetProperty, SetProperty_Syscall);
    RegisterSyscall(Syscall_GetProperty, GetProperty_Syscall);

    RegisterSyscall(Syscall_ManageSharedMemoryKey, ManageSharedMemoryKey_Syscall);
    RegisterSyscall(Syscall_SharedMemoryKeyAction, SharedMemoryKeyAction_Syscall);
}