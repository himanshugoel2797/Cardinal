#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"

void
RegisterAllSyscalls(void) {
    RegisterSyscall(Syscall_Nanosleep, Nanosleep_Syscall);
    RegisterSyscall(Syscall_ArchPrctl, ArchPrctl_Syscall);
    RegisterSyscall(Syscall_SetTidAddress, SetTidAddress_Syscall);
    RegisterSyscall(Syscall_MMap, MMap_Syscall);
    RegisterSyscall(Syscall_Brk, Brk_Syscall);
    RegisterSyscall(Syscall_RTSigProcMask, RTSigProcMask_Syscall);
    RegisterSyscall(Syscall_RTSigAction, RTSigAction_Syscall);
}