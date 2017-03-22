// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _LIB_CARDINAL_KEYMAN_H_
#define _LIB_CARDINAL_KEYMAN_H_

#include "syscall_list.h"

/**
 * \defgroup cap_api Capability Security System API
 * @{
 */

#define KEY_LENGTH 16

typedef struct {
    uint8_t key[KEY_LENGTH];
    uint64_t key_index;
    uint64_t machine_id;
    uint64_t network_id;
} Key_t;

#define KEY_STR_LEN (KEY_LENGTH * 2 + 3 * 16 + 1)

void
KeyToString(Key_t key, char *buf);

void
StringToKey(const char *buf, Key_t *key);

/**}@*/

#endif