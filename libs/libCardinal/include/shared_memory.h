#ifndef _CARDINAL_SHARED_MEMORY_H_
#define _CARDINAL_SHARED_MEMORY_H_

#include <stdint.h>

typedef enum {
    SharedMemoryFlags_Read = (0 << 0),
    SharedMemoryFlags_Write = (1 << 0),
    SharedMemoryFlags_Exec = (1 << 1),
    SharedMemoryFlags_PermissionMask = (SharedMemoryFlags_Write | SharedMemoryFlags_Exec | SharedMemoryFlags_Read),
    SharedMemoryFlags_Allocate = (1 << 2),
    SharedMemoryFlags_Map = (1 << 3),
    SharedMemoryFlags_Unmap = (0 << 3),
    SharedMemoryFlags_Free = (1 << 4),
    SharedMemoryFlags_Uncached = (1 << 5)
} SharedMemoryFlags;

#endif