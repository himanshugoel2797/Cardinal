// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _LIB_CARDINAL_KEYMAN_H_
#define _LIB_CARDINAL_KEYMAN_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_list.h"

/**
 * \defgroup cap_api Capability Security System API
 * @{
 */

#define KEY_LENGTH 16

typedef struct {
    uint8_t key[KEY_LENGTH];
    uint64_t key_index;
    uint32_t machine_id;
    uint32_t network_id;
} Key_t;

typedef enum {
    KeyType_SharedMemoryKey,
    KeyType_GeneralPurpose,
    KeyType_Signal,
    KeyType_Object,
} KeyType_t;

#define KEY_STR_LEN (KEY_LENGTH * (8 / 4) + 3 * (64 / 4) + 1)

void KeyToString(Key_t key, char *buf);

void StringToKey(const char *buf, Key_t *key);

#ifndef _KERNEL_

static __inline uint64_t GetKeyUsageCount(Key_t *key, uint64_t *cnt) {
    if (cnt == NULL) return -EINVAL;

    *cnt = Syscall1(Syscall_GetKeyUsageCount, (uint64_t)key);
    return GetErrno();
}

static __inline uint64_t RemoveKey(Key_t *key) {
    Syscall1(Syscall_RemoveKey, (uint64_t)key);
    return GetErrno();
}

#endif

/**}@*/

#endif