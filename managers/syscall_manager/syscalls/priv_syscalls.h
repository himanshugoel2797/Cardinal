#ifndef _CARDINAL_SYSCALLS_PRIV_H_
#define _CARDINAL_SYSCALLS_PRIV_H_

#include "types.h"

uint64_t
Nanosleep_Syscall(uint64_t instruction_pointer,
                  uint64_t syscall_num,
                  uint64_t *syscall_params);

uint64_t
ArchPrctl_Syscall(uint64_t instruction_pointer,
                  uint64_t syscall_num,
                  uint64_t *syscall_params);

uint64_t
SetTidAddress_Syscall(uint64_t instruction_pointer,
                      uint64_t syscall_num,
                      uint64_t *syscall_params);

uint64_t
MMap_Syscall(uint64_t instruction_pointer,
             uint64_t syscall_num,
             uint64_t *syscall_params);

uint64_t
Brk_Syscall(uint64_t instruction_pointer,
             uint64_t syscall_num,
             uint64_t *syscall_params);

#endif