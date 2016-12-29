#ifndef _CARDINAL_ROOT_SERVER_H_
#define _CARDINAL_ROOT_SERVER_H_

#include "../cardinal_types.h"
#include "../ipc.h"

#define MOUNT_PATH_LEN 4096

typedef enum {
    RootServerMessageType_Mount,
    RootServerMessageType_Unmount,
    RootServerMessageType_Retrieval
} RootServerMessageType;

typedef struct {
    Message m;
    RootServerMessageType MsgType;
    uint64_t path_key;
} RootMountRequest;

typedef struct {
    Message m;
    uint64_t result;
    UID pid;
} RootMountResponse;

typedef RootMountRequest RootUnmountRequest;
typedef RootMountResponse RootUnmountResponse;

typedef struct {
    uint64_t key;
    uint64_t vAddr;
    uint64_t len;
} MountKey;

uint64_t
Mount(char *path,
      uint32_t *key,
      MountKey *shmem_key);

uint64_t
Unmount(char *path,
        uint32_t *key,
        MountKey *shmem_key);

int
IsRootRequestReady(uint32_t key,  
                   MountKey *shmem_key,
                   uint64_t *error);

#endif