#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "managers.h"


uint64_t
SyscallSetErrno(uint64_t errno) {
	SetThreadErrno(GetCurrentThreadUID(), errno);
    return errno;
}

uint64_t
GetErrno_Syscall(uint64_t UNUSED(instruction_pointer),
                 uint64_t syscall_num,
                 uint64_t* UNUSED(syscall_params)) {

	if(syscall_num != Syscall_GetErrno)
		return -ENOSYS;

    return GetThreadErrno(GetCurrentThreadUID());
}