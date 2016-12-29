#ifndef _CARDINAL_PRIV_HELPERS_H_
#define _CARDINAL_PRIV_HELPERS_H_

#include "cardinal_types.h"
#include "shared_memory.h"
#include "syscall.h"
#include <string.h>

int
__card_setup_shmem(uint64_t *len,
            MemoryAllocationFlags target,
            uint64_t *key,
            uint64_t *vAddress);

#endif