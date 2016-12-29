#include <cardinal/root/server.h>
#include <cardinal/cardinal_types.h>
#include <cardinal/process.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    UID pid;
} RootEntry;

#define MOUNT_TABLE_SIZE MiB(1)
#define MOUNT_TABLE_LEN ((MOUNT_TABLE_SIZE / sizeof(RootEntry)) - 1)

typedef struct {
    uint32_t mount_index;
} PIDLookup;

#define PID_LOOKUP_SIZE (MOUNT_TABLE_LEN * sizeof(PIDLookup))

RootEntry *names;
PIDLookup *pids;

uint64_t
FNV1a_64(char *name) {
    uint64_t hash = 0xcbf29ce484222325;
    while(*name) {
        hash *= 0x100000001b3;
        hash ^= (uint64_t)*(name++);
    }
    return hash;
}

uint64_t
FNV1a_64_int(uint64_t val) {
    uint64_t hash = 0xcbf29ce484222325;
    while(val) {
        hash *= 0x100000001b3;
        hash ^= (val % 10) + '0';
        val /= 10;
    }
    return hash;
}

void
RootDir_Initialize(void) {
    R0_Map(GetCurrentProcessUID(),
           0,
           (uint64_t*)&names,
           MOUNT_TABLE_SIZE,
           CachingModeWriteBack,
           MemoryAllocationType_MMap | MemoryAllocationType_ReservedAllocation,
           MemoryAllocationFlags_Write | MemoryAllocationFlags_User | MemoryAllocationFlags_Present
          );

    R0_Map(GetCurrentProcessUID(),
           0,
           (uint64_t*)&pids,
           PID_LOOKUP_SIZE,
           CachingModeWriteBack,
           MemoryAllocationType_MMap | MemoryAllocationType_ReservedAllocation,
           MemoryAllocationFlags_Write | MemoryAllocationFlags_User | MemoryAllocationFlags_Present
          );
}

int
RootDir_AddMount(char *name, UID pid) {

    if(name == NULL | pid == 0)
        return -1;

    uint64_t pid_hash = FNV1a_64_int(pid);
    uint64_t pid_index = pid_hash % MOUNT_TABLE_LEN;

    if(pids[pid_index].mount_index != 0) {
        return -2;
    }

    uint64_t hash = FNV1a_64(name);
    uint64_t index = hash % MOUNT_TABLE_LEN;

    if(names[index].pid == 0) {
        names[index].pid = pid;
        pids[pid_index].mount_index = (uint32_t)index;
        return 0;
    } else
        return -3;
}

int
RootDir_GetMountPID(char *name, UID *pid) {

    if(pid == NULL | name == NULL)
        return -1;


    uint64_t hash = FNV1a_64(name);
    uint64_t index = hash % MOUNT_TABLE_LEN;
    if(names[index].pid != 0) {
        *pid = names[index].pid;
        return 0;
    } else
        return -1;
}

int
RootDir_RemoveMount(char *name, UID pid) {
    if(name == NULL | pid == 0)
        return -1;

    uint64_t pid_hash = FNV1a_64_int(pid);
    uint64_t pid_index = pid_hash % MOUNT_TABLE_LEN;

    if(pids[pid_index].mount_index == 0) {
        return -1;
    }

    uint64_t hash = FNV1a_64(name);
    uint64_t index = hash % MOUNT_TABLE_LEN;

    if(names[index].pid == pid) {
        names[index].pid = 0;
        pids[pid_index].mount_index = 0;
        return 0;
    } else
        return -1;
}

int
RootDir_RemoveProcess(UID pid) {

    if(pid == 0)
        return -1;

    uint64_t pid_hash = FNV1a_64_int(pid);
    uint64_t pid_index = pid_hash % MOUNT_TABLE_LEN;

    if(pids[pid_index].mount_index == 0) {
        return -1;
    }

    uint64_t index = pids[pid_index].mount_index;

    if(names[index].pid == pid) {
        names[index].pid = 0;
        pids[pid_index].mount_index = 0;
        return 0;
    } else
        return -1;
}