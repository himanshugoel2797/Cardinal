/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "helpers.h"


int
__card_setup_shmem(uint64_t *len,
                   MemoryAllocationFlags target,
                   Key_t *key,
                   uint64_t *vAddress) {

    if(*len % PAGE_SIZE)
        *len += PAGE_SIZE - *len % PAGE_SIZE;

    if(AllocateSharedMemory(*len,
                            CachingModeWriteBack,
                            MemoryAllocationType_MMap,
                            MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                            vAddress) != 0)
        return -1;

    if(GetSharedMemoryKey(*vAddress,
                          *len,
                          CachingModeWriteBack,
                          target | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                          key) != 0) {
        Unmap(*vAddress, *len);
        return -1;
    }

    memset((void*)*vAddress, 0, *len);

    return 0;
}


void
KeyToString(Key_t key, char *buf) {
    char *b0 = buf;
    char table[] = "0123456789abcdef";

    for(int i = 0; i < KEY_LENGTH; i++) {
        *(b0++) = table[key.key[i]/16 % 16];
        *(b0++) = table[key.key[i] % 16];
    }

    uint64_t val = key.key_index;
    for(int i = 15; i >= 0; i--) {
        *(b0++) = table[(val >> (i * 4)) & 0xF];
    }

    val = key.machine_id;
    for(int i = 15; i >= 0; i--) {
        *(b0++) = table[(val >> (i * 4)) & 0xF];
    }

    val = key.network_id;
    for(int i = 15; i >= 0; i--) {
        *(b0++) = table[(val >> (i * 4)) & 0xF];
    }

    *b0 = 0;
}

static int
hexToint(char a) {
    if(a - '0' < 10)
        return a - '0';
    else
        return (a - 'a') + 10;
}

void
StringToKey(const char *buf, Key_t *key) {
    const char *b0 = buf;
    for(int i = 0; i < KEY_LENGTH; i++) {
        key->key[i] = 0;
        key->key[i] += hexToint(*(b0++)) * 16;
        key->key[i] += hexToint(*(b0++));
    }

    uint64_t val = 0;
    for(int i = 15; i >= 0; i--)
        val += hexToint(*(b0++)) * (1 << (i * 4));
    key->key_index = val;

    val = 0;
    for(int i = 15; i >= 0; i--)
        val += hexToint(*(b0++)) * (1 << (i * 4));
    key->machine_id = val;

    val = 0;
    for(int i = 15; i >= 0; i--)
        val += hexToint(*(b0++)) * (1 << (i * 4));
    key->network_id = val;
}