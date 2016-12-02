#ifndef _CARDINAL_MEMORY_SERVER_H_
#define _CARDINAL_MEMORY_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <cardinal/cardinal_types.h>
#include <cardinal/memory.h>
#include <cardinal/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MemoryServerMessageType_MMapRequest,
    MemoryServerMessageType_GrantCreationRequest,
    MemoryServerMessageType_GrantRequest,
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

typedef enum {
    GrantRequestType_Apply,
    GrantRequestType_Unapply,
    GrantRequestType_Destroy,
} GrantRequestType;

typedef struct {
    Message m;
    MemoryServerMessageType MsgType;
    uint32_t Length;
    uint64_t Address;
} GrantCreationRequest;

typedef struct {
    Message m;
    uint64_t ReadKey;
    uint64_t WriteKey;
    MemoryAllocationErrors Error;
} GrantCreationResponse;

typedef struct {
    Message m;
    MemoryServerMessageType MsgType;
    GrantRequestType RequestType;
    uint64_t Key;
} GrantRequest;

typedef struct {
    Message m;
    uint64_t Address;
    MemoryAllocationErrors Error;
    MemoryAllocationFlags AccessPermissions;
} GrantResponse;

MemoryAllocationErrors
CreateGrant(uint64_t address,
            uint32_t length);

int
RequestCreateGrant(uint64_t address,
                   uint32_t length,
                   uint32_t *key);

int
IsGrantCreated(uint32_t key,
               MemoryAllocationErrors *error,
               uint64_t *read_key,
               uint64_t *write_key);


MemoryAllocationErrors
ApplyGrant(uint64_t key);

int
RequestApplyGrant(uint64_t grant_key,
                  uint32_t *key);

int
IsGrantApplied(uint32_t key,
               MemoryAllocationErrors *error,
               uint64_t *address,
               MemoryAllocationFlags *access_perms);


MemoryAllocationErrors
UnapplyGrant(uint64_t key);

int
RequestUnapplyGrant(uint64_t grant_key,
                    uint32_t *key);

int
IsGrantUnapplied(uint32_t key,
                 MemoryAllocationErrors *error);

MemoryAllocationErrors
DestroyGrant(uint64_t key);

int
RequestDestroyGrant(uint64_t grant_key,
                    uint32_t *key);

int
IsGrantDestroyed(uint32_t key,
                 MemoryAllocationErrors *error);

#ifdef __cplusplus
}
#endif
#endif