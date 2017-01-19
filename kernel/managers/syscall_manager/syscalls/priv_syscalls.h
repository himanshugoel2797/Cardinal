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
GetErrno_Syscall(void);

uint64_t
Nanosleep_Syscall(uint64_t time_ns);

uint64_t
Brk_Syscall(void *targ_brk_address);

uint64_t
PostToProcess_Syscall(uint64_t dst_pid,
                      uint64_t message_p0,
                      uint64_t message_p1,
                      uint64_t message_p2,
                      uint64_t message_p3);

uint64_t
GetMainRingBuffer(void** addr, 
                  size_t *sz);

uint64_t
SleepOnLocation(void *addr, 
                int op, 
                uint64_t val, 
                uint64_t timeout_ns);

uint64_t
SetProperty_Syscall(uint64_t property,
                    uint64_t sub_property,
                    uint64_t value);

uint64_t
GetProperty_Syscall(uint64_t property,
                    uint64_t sub_property);

uint64_t
R0_Map_Syscall(struct MemoryMapParams *mmap_params);

uint64_t
R0_Unmap_Syscall(UID pid,
                uint64_t addr,
                uint64_t size);

uint64_t
R0_AllocatePages_Syscall(uint64_t page_cnt,
                        PhysicalMemoryAllocationFlags flags);

uint64_t
R0_FreePages_Syscall(uint64_t addr,
                     uint64_t size);

uint64_t
R0_CreateProcess_Syscall(UID parent,
                         uint64_t gid);

uint64_t
R0_CreateThread_Syscall(UID parent,
                        ThreadEntryPoint entry_point,
                        void* arg);

uint64_t
R0_StartProcess_Syscall(UID pid);

uint64_t
R0_GetBootInfo_Syscall(int syscall_num,
                      void *copy_dst);

uint64_t
R01_GetPhysicalAddress_Syscall(UID pid,
                               void* addr);

uint64_t
Unmap_Syscall(uint64_t addr,
              uint64_t size);

uint64_t
AllocateSharedMemory_Syscall(uint64_t length,
                             CachingMode cacheMode,
                             void* unused,
                             MemoryAllocationFlags flags);

uint64_t
GetSharedMemoryKey_Syscall(uint64_t vAddress,
                           uint64_t length,
                           CachingMode cacheMode,
                           MemoryAllocationFlags flags);

uint64_t
ApplySharedMemoryKey_Syscall(uint64_t key,
                             void *shmem_data_p);

uint64_t
FreeSharedMemoryKey_Syscall(uint64_t key);

uint64_t
R0_AllocateSharedMemory_Syscall(uint64_t length,
                                CachingMode cacheMode,
                                MemoryAllocationType allocType,
                                MemoryAllocationFlags flags,
                                uint64_t phys_addr);

uint64_t
GetSharedMemoryKeyUsageCount_Syscall(uint64_t key);

uint64_t
R01_RegisterForInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                                 uint64_t syscall_num,
                                 uint64_t *syscall_params);

uint64_t
R01_AllocateInterrupts_Syscall(uint64_t UNUSED(instruction_pointer),
                              uint64_t syscall_nuW"m,
                              uint64_t *syscall_params);

uint64_t
R0_KillProcess_Syscall(UID pid,
                       uint32_t exit_code);

#endif