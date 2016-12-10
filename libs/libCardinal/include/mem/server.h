#ifndef _CARDINAL_MEMORY_SERVER_H_
#define _CARDINAL_MEMORY_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include "../cardinal_types.h"
#include "../memory.h"
#include "../ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MemoryServerMessageType_MMapRequest,
} MemoryServerMessageType;

typedef enum {
    MMapFlags_None,
    MMapFlags_Fixed
} MMapFlags;

typedef struct {
    Message m;
    MemoryServerMessageType MsgType;
    uint64_t address;
    uint64_t size;
    CachingMode cachingMode;
    MemoryAllocationFlags flags;
    MMapFlags mmap_flags;
} MMapRequest;

typedef struct {
    Message m;
    uint64_t address;
    MemoryAllocationErrors error;
} MMapResponse;

MemoryAllocationErrors
MMap(uint64_t *address,
     uint64_t size,
     MemoryAllocationFlags flags,
     MMapFlags m_flags,
     CachingMode cacheMode);

int
RequestMMap(uint64_t address,
            uint64_t size,
            MemoryAllocationFlags flags,
            MMapFlags m_flags,
            CachingMode cacheMode,
            uint32_t *key);

int
IsMMapped(uint32_t id,
          MemoryAllocationErrors *error,
          uint64_t *address);


#ifdef __cplusplus
}
#endif
#endif