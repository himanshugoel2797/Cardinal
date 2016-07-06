#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"

void
RegisterAllSyscalls(void)
{
	RegisterSyscall(Syscall_Nanosleep, Nanosleep_Syscall);
	RegisterSyscall(Syscall_ArchPrctl, ArchPrctl_Syscall);
}