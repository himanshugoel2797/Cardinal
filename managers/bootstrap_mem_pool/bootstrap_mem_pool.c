#include "bootstrap_mem_pool.h"
#include "managers.h"

static uint8_t mem_pool[BOOTSTRAP_MEM_POOL];
static uint32_t pos = 0;

void*
bootstrap_malloc(size_t size)
{
    //Make sure size is 32bit aligned
    size += (size % 4);
    if( (pos + size) > BOOTSTRAP_MEM_POOL )
        return NULL;

    void *ptr = &mem_pool[pos];
    pos += size;
    return ptr;
}
