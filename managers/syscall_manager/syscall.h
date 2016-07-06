#ifndef _CARDINAL_MAN_SYSCALL_H_
#define _CARDINAL_MAN_SYSCALL_H_

#include "types.h"

#define MAX_SYSCALL_COUNT 256

typedef struct {
    uint64_t size;
    uint64_t *params;
    uint64_t param_num;
} SyscallData;

typedef uint64_t (*SyscallHandler)(uint64_t instruction_pointer,
                                   uint64_t syscall_num,
                                   uint64_t* syscall_params);

extern SyscallHandler Syscalls[MAX_SYSCALL_COUNT];

void
SyscallMan_Initialize(void);

uint64_t
SyscallReceived(uint64_t instruction_pointer,
                uint64_t syscall_num,
                uint64_t *syscall_params,
                uint64_t syscall_param_cnt);


uint64_t
AllocateSyscall(void);

void
RegisterSyscall(uint64_t syscall_num,
                SyscallHandler handler);

#endif