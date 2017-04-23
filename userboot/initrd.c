/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdint.h>
#include <stdbool.h>

#include "initrd.h"

#include <cardinal/bootinfo.h>
#include <cardinal/memory.h>
#include <cardinal/syscall_property.h>

static uint64_t InitrdStartAddress = 0;

typedef struct TARHeader {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
} TARHeader;

unsigned int getsize(const char *in) {

    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;

    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

int
strncmp(const char* s1,
        const char* s2,
        size_t n) {
    while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}

size_t
strlen(const char *str) {
    size_t size = 0;
    while(str[size] != 0) {
        size++;
    }
    return size;
}

bool
GetFile(const char *file,
        void **loc,
        size_t *size) {
    *loc = NULL;
    *size = 0;

    TARHeader *file_entry = (TARHeader*)InitrdStartAddress;

    while(file_entry != NULL && file_entry->filename[0] != 0) {
        uint32_t len = strlen(file_entry->filename);

        if(strlen(file) == len && strncmp(file_entry->filename, file, len) == 0) {
            *loc = (void*)((uint64_t)file_entry + 512);
            *size = getsize(file_entry->size);
            break;
        }
        file_entry = (TARHeader*)((uint64_t)file_entry + 512 + getsize(file_entry->size));

        if((uint64_t)file_entry % 512) {
            file_entry = (TARHeader*)((uint64_t)file_entry + (512 - (uint64_t)file_entry % 512));
        }
    }

    if(*loc == NULL)return 0;
    return 1;
}

int
ImportInitrd(void) {
    CardinalBootInfo b_info;
    if(R0_GetBootInfo(&b_info) != 0)
        return -1;

    uint64_t initrd_addr = 0;
    uint64_t pid = 0;

    if(GetProperty(CardinalProperty_PID, 0, &pid) != 0)
        return -1;

    if(R01_GetPhysicalAddress(pid, b_info.InitrdStartAddress, &initrd_addr) != 0)
        return -1;

    uint64_t InitrdLength = b_info.InitrdLength;

    InitrdStartAddress = 0;

    if(R0_Map(pid,
              initrd_addr,
              &InitrdStartAddress,
              InitrdLength,
              CachingModeWriteBack,
              MemoryAllocationType_MMap | MemoryAllocationType_Phys,
              MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Read | MemoryAllocationFlags_User | MemoryAllocationFlags_Present) != 0)
        return -1;

    return 0;
}
