#ifndef _CARDINAL_SYSCALLS_PRIV_H_
#define _CARDINAL_SYSCALLS_PRIV_H_

#include "types.h"

void
MemoryInitLocks(void);

void
PropertyInitLocks(void);

uint64_t
SyscallSetErrno(uint64_t errno);

uint64_t
GetErrno_Syscall(uint64_t instruction_pointer,
                 uint64_t syscall_num,
                 uint64_t *syscall_params);

uint64_t
Nanosleep_Syscall(uint64_t instruction_pointer,
                  uint64_t syscall_num,
                  uint64_t *syscall_params);

uint64_t
Brk_Syscall(uint64_t instruction_pointer,
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
SetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

uint64_t
GetProperty_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

uint64_t
R0Map_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params);

uint64_t
R0Unmap_Syscall(uint64_t UNUSED(instruction_pointer),
                uint64_t syscall_num,
                uint64_t *syscall_params);

uint64_t
R0AllocatePages_Syscall(uint64_t UNUSED(instruction_pointer),
                        uint64_t syscall_num,
                        uint64_t *syscall_params);

uint64_t
R0FreePages_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params);

uint64_t
R0CreateProcess_Syscall(uint64_t UNUSED(instruction_pointer),
                        uint64_t syscall_num,
                        uint64_t *syscall_params);

uint64_t
R0StartProcess_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params);

uint64_t
R0CreateThread_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params);

uint64_t
R0GetBootInfo_Syscall(uint64_t UNUSED(instruction_pointer),
                      uint64_t syscall_num,
                      uint64_t *syscall_params);

uint64_t
R0GetPhysicalAddress_Syscall(uint64_t UNUSED(instruction_pointer),
                             uint64_t syscall_num,
                             uint64_t *syscall_params);

uint64_t
Unmap_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params);

uint64_t
AllocateSharedMemory_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params);

uint64_t
GetSharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params);

uint64_t
ApplySharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params);

uint64_t
FreeSharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params);
#endif