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

    RegisterSyscall(Syscall_R0_AllocatePages, R0AllocatePages_Syscall);
    RegisterSyscall(Syscall_R0_FreePages, R0FreePages_Syscall);

    RegisterSyscall(Syscall_R0_CreateProcess, R0CreateProcess_Syscall);
    RegisterSyscall(Syscall_R0_StartProcess, R0StartProcess_Syscall);
    RegisterSyscall(Syscall_R0_CreateThread, R0CreateThread_Syscall);
}