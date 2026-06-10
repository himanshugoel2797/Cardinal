/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "libs/libCardinal/include/keyman.h"
#include "libs/libCardinal/include/shared_memory.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "priv_syscalls.h"
#include "syscalls_all.h"

#include "managers.h"

uint64_t GetKeyUsageCount_Syscall(Key_t* key) {
  uint64_t cnt = 0;

  if (key == NULL) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  Key_t localKey = *key;

  KeyManagerErrors err = KeyMan_GetKeyUsageCount(&localKey, &cnt);

  if (err != KeyManagerErrors_None) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  SyscallSetErrno(0);
  return cnt;
}

// TODO(key-system): CreateKey_Syscall is registered (syscall 21) and declared
// in priv_syscalls.h with the 5-arg signature below, but its implementation was
// left commented out mid-refactor. This stub resolves the link and reports
// failure until the key-allocation path is reimplemented in the key-system PR.
// The original (incomplete) 4-arg body is preserved at the end of this comment.
uint64_t CreateKey_Syscall(KeyType_t type, uint32_t flags, void* params,
                           Key_t* key, uint32_t* key_cnt) {
  (void)type;
  (void)flags;
  (void)params;
  (void)key;
  (void)key_cnt;
  SyscallSetErrno(-EINVAL);
  return -1;
}

/*
uint64_t CreateKey_Syscall(uint64_t v0, uint64_t v1, uint64_t v2, Key_t* key) {
    if (key == NULL) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    uint64_t idents[IDENTIFIER_COUNT];
    idents[KeyIdentifier_V0] = v0;
    idents[KeyIdentifier_V1] = v1;
    idents[KeyIdentifier_V2] = v2;
    idents[KeyIdentifier_Count] = 0;
    idents[KeyIdentifier_Type] = KeyType_GeneralPurpose;

    KeyManagerErrors err = KeyMan_AllocateKey(idents, key);

    if (err != KeyManagerErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return 0;
}*/

uint64_t AddKey_Syscall(Key_t* key) {
  if (key == NULL) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  Key_t localKey = *key;
  int idx = -1;
  ThreadError err = AddKey(GetCurrentProcessUID(), &localKey, &idx);

  if (err != ThreadError_None) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  SyscallSetErrno(0);
  return idx;
}

uint64_t RemoveKey_Syscall(int32_t key) {
  ThreadError err = RemoveKey(GetCurrentProcessUID(), key);

  if (err != ThreadError_None) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  SyscallSetErrno(0);
  return 0;
}

uint64_t GetKeyIndex_Syscall(UID pid, Key_t* key) {
  if (key == NULL) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  Key_t localKey = *key;

  int idx = -1;
  ThreadError err = GetKeyIndex(pid, &localKey, &idx);

  if (err != ThreadError_None) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  SyscallSetErrno(0);
  return idx;
}

uint64_t UseKey_Syscall(Key_t* key) {
  if (key == NULL) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  Key_t localKey = *key;
  KeyManagerErrors err = KeyMan_UseKey(&localKey);

  if (err != KeyManagerErrors_None) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  SyscallSetErrno(0);
  return 0;
}