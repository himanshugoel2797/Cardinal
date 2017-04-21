/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "libs/libCardinal/include/shared_memory.h"
#include "libs/libCardinal/include/keyman.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "priv_syscalls.h"
#include "syscalls_all.h"

#include "managers.h"

uint64_t GetKeyUsageCount_Syscall(Key_t *key) {
    uint64_t cnt = 0;

    KeyManagerErrors err = KeyMan_GetKeyUsageCount(key, &cnt);

    if (err != KeyManagerErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    SyscallSetErrno(0);
    return cnt;
}