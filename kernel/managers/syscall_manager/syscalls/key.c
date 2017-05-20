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
}

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