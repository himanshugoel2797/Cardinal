#ifndef _CARDINAL_MAN_SYSCALL_H_
#define _CARDINAL_MAN_SYSCALL_H_

#include "types.h"

#define MAX_SYSCALL_COUNT 256

typedef void (*SyscallHandler)(uint64_t instruction_pointer,
                               uint64_t syscall_num,
                               uint64_t* syscall_params);

extern SyscallHandler Syscalls[MAX_SYSCALL_COUNT];

void
SyscallReceived(uint64_t instruction_pointer,
                uint64_t syscall_num,
                uint64_t *syscall_params);


uint64_t
AllocateSyscall(void);

void
RegisterSyscall(uint64_t syscall_num,
				SyscallHandler handler);

#endif