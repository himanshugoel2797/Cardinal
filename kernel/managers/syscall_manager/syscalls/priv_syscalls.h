// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _CARDINAL_SYSCALLS_PRIV_H_
#define _CARDINAL_SYSCALLS_PRIV_H_

#include "types.h"
#include "thread.h"
#include "libs/libCardinal/include/keyman.h"

void MemoryInitLocks(void);

void PropertyInitLocks(void);

uint64_t SyscallSetErrno(uint64_t errno);

uint64_t GetErrno_Syscall(void);

uint64_t Nanosleep_Syscall(uint64_t time_ns);

uint64_t Brk_Syscall(void* targ_brk_address);

uint64_t PostToProcess_Syscall(uint64_t dst_pid,
                               uint128_t *msg);

uint64_t GetMainRingBuffer(void** addr, size_t* sz);

uint64_t SleepOnLocation(void* addr, int op, uint64_t val, uint64_t timeout_ns);

uint64_t SetProperty_Syscall(uint64_t property,
                             uint64_t sub_property,
                             uint64_t value);

uint64_t GetProperty_Syscall(uint64_t property, uint64_t sub_property);

uint64_t R0_Map_Syscall(struct MemoryMapParams* mmap_params);

uint64_t R0_Unmap_Syscall(UID pid, uint64_t addr, uint64_t size);

uint64_t R0_AllocatePages_Syscall(uint64_t page_cnt,
                                  PhysicalMemoryAllocationFlags flags);

uint64_t R0_FreePages_Syscall(uint64_t addr, uint64_t size);

uint64_t R0_CreateProcess_Syscall(UID parent, uint64_t gid);

uint64_t R0_CreateThread_Syscall(UID parent,
                                 ThreadEntryPoint entry_point,
                                 void* arg);

uint64_t R0_StartProcess_Syscall(UID pid);

uint64_t R0_GetBootInfo_Syscall(int syscall_num, void* copy_dst);

uint64_t R01_GetPhysicalAddress_Syscall(UID pid, void* addr);

uint64_t Unmap_Syscall(uint64_t addr, uint64_t size);

uint64_t AllocateSharedMemory_Syscall(uint64_t length,
                                      CachingMode cacheMode,
                                      void* UNUSED(unused),
                                      MemoryAllocationFlags flags);

uint64_t GetSharedMemoryKey_Syscall(uint64_t vAddress,
                                    uint64_t length,
                                    CachingMode cacheMode,
                                    MemoryAllocationFlags flags,
                                    Key_t *key);

uint64_t ApplySharedMemoryKey_Syscall(Key_t *key, void* shmem_data_p);

uint64_t FreeSharedMemoryKey_Syscall(Key_t *key);

uint64_t R0_AllocateSharedMemory_Syscall(uint64_t length,
                                         CachingMode cacheMode,
                                         MemoryAllocationType allocType,
                                         MemoryAllocationFlags flags,
                                         uint64_t phys_addr);

uint64_t GetSharedMemoryKeyUsageCount_Syscall(Key_t *key);

uint64_t R01_RegisterForInterrupts_Syscall(uint64_t p0,
                                           uint64_t p1);

uint64_t R01_AllocateInterrupts_Syscall(int cnt);

uint64_t R0_KillProcess_Syscall(UID pid, uint32_t exit_code);

#endif