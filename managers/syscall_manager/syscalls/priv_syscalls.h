/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
GetIPCMessageMsgType_Syscall(uint64_t UNUSED(instruction_pointer),
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
R01GetPhysicalAddress_Syscall(uint64_t UNUSED(instruction_pointer),
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

uint64_t
R0AllocateSharedMemory_Syscall(uint64_t UNUSED(instruction_pointer),
                               uint64_t syscall_num,
                               uint64_t *syscall_params);

uint64_t
GetSharedMemoryKeyUsageCount_Syscall(uint64_t UNUSED(instruction_pointer),
                                     uint64_t syscall_num,
                                     uint64_t *syscall_params);

uint64_t
WaitForMessage_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params);

uint64_t
R01RegisterForInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                                 uint64_t syscall_num,
                                 uint64_t *syscall_params);

uint64_t
R01AllocateInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                              uint64_t syscall_num,
                              uint64_t *syscall_params);

uint64_t
R0KillProcess_Syscall(uint64_t UNUSED(instruction_pointer),
                      uint64_t syscall_num,
                      uint64_t *syscall_params);

#endif