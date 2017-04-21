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


uint64_t Nanosleep_Syscall(uint64_t time_ns);

uint64_t R0_GetBootInfo_Syscall(void* copy_dst);


uint64_t SetProperty_Syscall(uint64_t property,
                             uint64_t sub_property,
                             uint64_t value);

uint64_t GetProperty_Syscall(uint64_t property, uint64_t sub_property);


uint64_t GetErrno_Syscall(void);


uint64_t AllocateSharedMemory_Syscall(uint64_t length,
                                      CachingMode cacheMode,
                                      void* UNUSED(unused),
                                      MemoryAllocationFlags flags);

uint64_t R0_AllocateSharedMemory_Syscall(uint64_t length,
        CachingMode cacheMode,
        MemoryAllocationType allocType,
        MemoryAllocationFlags flags,
        uint64_t phys_addr);

uint64_t ApplySharedMemoryKey_Syscall(Key_t *key, void* shmem_data_p);

uint64_t GetSharedMemoryKey_Syscall(uint64_t vAddress,
                                    uint64_t length,
                                    CachingMode cacheMode,
                                    MemoryAllocationFlags flags,
                                    Key_t *key);

uint64_t R0_Map_Syscall(struct MemoryMapParams* mmap_params);

uint64_t R0_Unmap_Syscall(UID pid, uint64_t addr, uint64_t size);

uint64_t Unmap_Syscall(uint64_t addr, uint64_t size);

uint64_t Brk_Syscall(void* targ_brk_address);


uint64_t R0_AllocatePages_Syscall(uint64_t page_cnt,
                                  PhysicalMemoryAllocationFlags flags);

uint64_t R0_FreePages_Syscall(uint64_t addr, uint64_t size);

uint64_t R01_GetPhysicalAddress_Syscall(UID pid, void* addr);


uint64_t CreateProcess_Syscall(void *prog, uint32_t *keys, char *argv[], int argc);


uint64_t R0_GetThreadInfo_Syscall(void);        //TODO

uint64_t CreateThread_Syscall(UID parent,
                              ThreadEntryPoint entry_point,
                              void* arg);

uint64_t KillThread_Syscall(void);              //TODO

uint64_t ExitDeleteThread_Syscall(void);        //TODO

uint64_t SetThreadIsPaused_Syscall(void);        //TODO


uint64_t R01_AllocateInterrupts_Syscall(int cnt);

uint64_t R01_RegisterForInterrupts_Syscall(uint64_t p0,
        uint64_t p1);


uint64_t WaitSignal_Syscall(void);

uint64_t HandleSignal_Syscall(void);

uint64_t Signal_Syscall(void);


uint64_t RegisterName_Syscall(void);

uint64_t UnregisterName_Syscall(void);


uint64_t CreateKey_Syscall(void);

uint64_t AddKey_Syscall(void);

uint64_t RemoveKey_Syscall(void);

uint64_t HasKey_Syscall(void);

uint64_t UseKey_Syscall(void);

uint64_t GetKeyUsageCount_Syscall(Key_t *key);

#endif