#ifndef _BLOCK_ALLOC_H_
#define _BLOCK_ALLOC_H_

#include "types.h"

//Disable the large heap
#define LARGE_HEAP_MEM_SIZE 0

//Size of a single block in the chain
#define LARGE_HEAP_BLOCK_SIZE KiB(4)
#define LARGE_HEAP_BLOCK_COUNT (LARGE_HEAP_MEM_SIZE/LARGE_HEAP_BLOCK_SIZE)

//Allocate 512MB of memory for the small heap
#define SMALL_HEAP_MEM_SIZE MiB(64)
#define SMALL_HEAP_BLOCK_SIZE 32
#define SMALL_HEAP_BLOCK_COUNT (SMALL_HEAP_MEM_SIZE/SMALL_HEAP_BLOCK_SIZE)

#define MAX_SIMULTANEOUS_LARGE_ALLOCS LARGE_HEAP_BLOCK_COUNT
#define MAX_SIMULTANEOUS_SMALL_ALLOCS SMALL_HEAP_BLOCK_COUNT

#if ENGINE_DEBUG
#define MEMORY_LEAK_CHECK_BLOCK_SIZE 16
#endif

typedef enum {
    BLOCK_TYPE_FREE = 0,
    BLOCK_TYPE_ALLOC_MID = 1,
    BLOCK_TYPE_ALLOC_START = 2,
    BLOCK_TYPE_ALLOC_END = 4,
    BLOCK_TYPE_ALLOC_START_END = (BLOCK_TYPE_ALLOC_START | BLOCK_TYPE_ALLOC_END)
} BLOCK_TYPE;

int
Balloc_Initialize(void);

UID
Balloc_Alloc(size_t size);

void*
Balloc_GetBaseAddress(UID blockID);

UID
Balloc_GetUID(void *baseAddress);

void
Balloc_Free(UID blockID);

#endif