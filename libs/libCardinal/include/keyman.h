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
    uint8_t key[16];
    uint64_t key_index;
    uint64_t machine_id;
    uint64_t network_id;
} Key_t;


/**}@*/

#endif