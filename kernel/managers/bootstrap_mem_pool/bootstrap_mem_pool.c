/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "bootstrap_mem_pool.h"
#include "managers.h"
#include "common/common.h"

static uint8_t mem_pool[BOOTSTRAP_MEM_POOL];
static volatile uint32_t pos = 0;

void* bootstrap_malloc(size_t size) {
    // Make sure size is 32bit aligned
    if (size % 4 != 0)
        size = (size / 4 + 1) * 4;
    ASSERT((pos + size) <= BOOTSTRAP_MEM_POOL, "Bootstrap memory pool exhausted.");

    void* ptr = &mem_pool[pos];
    pos += size;

    memset(ptr, 0, size);
    return ptr;
}
