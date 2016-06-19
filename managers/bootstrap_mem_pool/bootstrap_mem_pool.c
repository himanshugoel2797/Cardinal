#include "bootstrap_mem_pool.h"
#include "managers.h"
#include "common/common.h"

static uint8_t mem_pool[BOOTSTRAP_MEM_POOL];
static uint32_t pos = 0;

void*
bootstrap_malloc(size_t size) {
    //Make sure size is 32bit aligned
    if(size % 4 != 0)size = (size/4 + 1) * 4;
    if( (pos + size) > BOOTSTRAP_MEM_POOL )
        __asm__("cli\n\thlt");

    void *ptr = &mem_pool[pos];
    pos += size;

    memset(ptr, 0, size);
    return ptr;
}
