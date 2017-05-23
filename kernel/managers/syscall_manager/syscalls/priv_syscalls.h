// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _CARDINAL_SYSCALLS_PRIV_H_
#define _CARDINAL_SYSCALLS_PRIV_H_

#include "libs/libCardinal/include/keyman.h"
#include "thread.h"
#include "types.h"

void MemoryInitLocks(void);

void PropertyInitLocks(void);

uint64_t SyscallSetErrno(uint64_t errno);

uint64_t Nanosleep_Syscall(uint64_t time_ns);

uint64_t R0_GetBootInfo_Syscall(void* copy_dst);

uint64_t SetProperty_Syscall(uint64_t property, uint64_t sub_property,
                             uint64_t value);

uint64_t GetProperty_Syscall(uint64_t property, uint64_t sub_property);

uint64_t GetErrno_Syscall(void);

uint64_t Brk_Syscall(void* targ_brk_address);

uint64_t R01_GetPhysicalAddress_Syscall(UID pid, void* addr);

uint64_t CreateProcess_Syscall(void* prog, uint64_t sz, uint32_t* keys,
                               char* argv[], int argc);

uint64_t CreateThread_Syscall(UID parent, ThreadEntryPoint entry_point,
                              void* arg);

uint64_t KillThread_Syscall(UID tid);  // TODO

uint64_t ExitDeleteThread_Syscall(uint64_t exitCode);  // TODO

uint64_t WaitSignal_Syscall(Key_t* signalVal);

uint64_t HandleSignal_Syscall(Key_t* signalVal);

uint64_t Signal_Syscall(Key_t* signalVal, void* param);  // param is sizeof
// Key_t

uint64_t TryLockKey_Syscall(Key_t* key);

uint64_t UnlockKey_Syscall(Key_t* key);

uint64_t RegisterTagProvider_Syscall(char* name, Key_t* signals);

uint64_t GetWithTag_Syscall(const char* cond, char* tags,
                            uint32_t flags);  // TODO: redesign this to know the
// length of the tags, flags to
// control TAG list mode or FILE list
// mode.

uint64_t Read_Syscall(Key_t* key, void* buff, uint32_t flags);

uint64_t Write_Syscall(Key_t* key, void* buff, uint64_t len, uint32_t flags);

uint64_t CreateKey_Syscall(KeyType_t type, uint32_t flags, void* params,
                           Key_t* key, uint32_t* key_cnt);

uint64_t AddKey_Syscall(Key_t* key);

uint64_t Dup_Syscall(int32_t src, int32_t dst);

uint64_t RemoveKey_Syscall(int32_t key);

uint64_t GetKeyIndex_Syscall(UID pid, Key_t* key);

uint64_t UseKey_Syscall(Key_t* key);

uint64_t GetKeyUsageCount_Syscall(Key_t* key);

#endif