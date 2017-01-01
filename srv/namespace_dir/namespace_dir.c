/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/namespace/server.h>
#include <cardinal/cardinal_types.h>
#include <cardinal/process.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    UID pid;
} NamespaceEntry;

#define NAMESPACE_TABLE_SIZE MiB(1)
#define NAMESPACE_TABLE_LEN ((NAMESPACE_TABLE_SIZE / sizeof(NamespaceEntry)) - 1)

typedef struct {
    uint32_t namesapce_index;
} PIDLookup;

#define PID_LOOKUP_SIZE (NAMESPACE_TABLE_LEN * sizeof(PIDLookup))

NamespaceEntry *names;
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
NamespaceDir_Initialize(void) {
    R0_Map(GetCurrentProcessUID(),
           0,
           (uint64_t*)&names,
           NAMESPACE_TABLE_SIZE,
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
NamespaceDir_AddNamespace(char *name, UID pid) {

    if(name == NULL | pid == 0)
        return -1;

    uint64_t pid_hash = FNV1a_64_int(pid);
    uint64_t pid_index = pid_hash % NAMESPACE_TABLE_LEN;

    if(pids[pid_index].namesapce_index != 0) {
        return -2;
    }

    uint64_t hash = FNV1a_64(name);
    uint64_t index = hash % NAMESPACE_TABLE_LEN;

    if(names[index].pid == 0) {
        names[index].pid = pid;
        pids[pid_index].namesapce_index = (uint32_t)index;
        return 0;
    } else
        return -3;
}

int
NamespaceDir_GetNamespacePID(char *name, UID *pid) {

    if(pid == NULL | name == NULL)
        return -1;


    uint64_t hash = FNV1a_64(name);
    uint64_t index = hash % NAMESPACE_TABLE_LEN;
    if(names[index].pid != 0) {
        *pid = names[index].pid;
        return 0;
    } else
        return -1;
}

int
NamespaceDir_RemoveNamespace(char *name, UID pid) {
    if(name == NULL | pid == 0)
        return -1;

    uint64_t pid_hash = FNV1a_64_int(pid);
    uint64_t pid_index = pid_hash % NAMESPACE_TABLE_LEN;

    if(pids[pid_index].namesapce_index == 0) {
        return -1;
    }

    uint64_t hash = FNV1a_64(name);
    uint64_t index = hash % NAMESPACE_TABLE_LEN;

    if(names[index].pid == pid) {
        names[index].pid = 0;
        pids[pid_index].namesapce_index = 0;
        return 0;
    } else
        return -1;
}

int
NamespaceDir_RemoveProcess(UID pid) {

    if(pid == 0)
        return -1;

    uint64_t pid_hash = FNV1a_64_int(pid);
    uint64_t pid_index = pid_hash % NAMESPACE_TABLE_LEN;

    if(pids[pid_index].namesapce_index == 0) {
        return -1;
    }

    uint64_t index = pids[pid_index].namesapce_index;

    if(names[index].pid == pid) {
        names[index].pid = 0;
        pids[pid_index].namesapce_index = 0;
        return 0;
    } else
        return -1;
}