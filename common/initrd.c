#include "initrd.h"
#include "boot_information/boot_information.h"
#include "common.h"

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

bool
Initrd_GetFile(const char *file,
               void **loc,
               uint64_t *size) {

    CardinalBootInfo* bootInfo = GetBootInfo();
    if(bootInfo->initrd_start_addr == 0 | bootInfo->initrd_len == 0)return FALSE;

    *loc = NULL;
    *size = 0;

    TARHeader *file_entry = (TARHeader*)bootInfo->initrd_start_addr;

    while(file_entry->filename[0] != 0) {
        uint32_t len = strlen(file_entry->filename);

        if(strlen(file) == len && strncmp(file_entry->filename, file, len) == 0) {
            *loc = (void*)((uint64_t)file_entry + 512);
            *size = getsize(file_entry->size);
            break;
        }
        file_entry = (TARHeader*)((uint64_t)file_entry + 512 + getsize(file_entry->size));
        
        if((uint64_t)file_entry % 512 != 0) {
            uint64_t tmp_fe = (uint64_t)file_entry;
            tmp_fe += 512 - (tmp_fe % 512);
            file_entry = (TARHeader*)tmp_fe;
        }
    }

    if(*loc == NULL)return FALSE;
    return TRUE;
}