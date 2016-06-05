#ifndef _CARDINAL_MAN_SYSCALL_H_
#define _CARDINAL_MAN_SYSCALL_H_

#include "types.h"

void
SyscallReceived(uint64_t instruction_pointer,
                uint64_t syscall_num,
                uint64_t *syscall_params);

#endif