#ifndef _CARDINAL_SYSCALLS_PRIV_H_
#define _CARDINAL_SYSCALLS_PRIV_H_

#include "types.h"

uint64_t
Nanosleep_Syscall(uint64_t instruction_pointer,
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

uint64_t
Clone_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params);

uint64_t
Fork_Syscall(uint64_t UNUSED(instruction_pointer),
             uint64_t syscall_num,
             uint64_t *syscall_params);

uint64_t
GetIPCMessageFrom_Syscall(uint64_t UNUSED(instruction_pointer),
                          uint64_t syscall_num,
                          uint64_t *syscall_params);

uint64_t
PostIPCMessage_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params);

uint64_t
RegisterIPCMessageHandler_Syscall(uint64_t UNUSED(instruction_pointer),
                                  uint64_t syscall_num,
                                  uint64_t *syscall_params);

uint64_t
SetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

uint64_t
GetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

uint64_t
ManageSharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

uint64_t
SharedMemoryKeyAction_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

uint64_t
R0MemoryMap_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

#endif