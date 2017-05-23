/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "shared_memory.h"

uint64_t KeySys_Shmem_Read(Key_t* key, void* buff, uint32_t flags) {}

uint64_t KeySys_Shmem_Write(Key_t* key, void* buff, uint64_t len,
                            uint32_t flags) {}

uint64_t KeySys_Shmem_CreateKey(KeyType_t type, uint32_t flags, void* params,
                                Key_t* key, uint32_t* key_cnt) {}