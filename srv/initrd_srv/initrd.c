/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <stdint.h>
#include <stdbool.h>

#include "initrd.h"

#include <cardinal/bootinfo.h>
#include <cardinal/memory.h>
#include <cardinal/syscall_property.h>
#include <string.h>

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

bool
GetFile(const char *file,
        void **loc,
        size_t *size) {
    *loc = NULL;
    *size = 0;

    TARHeader *file_entry = (TARHeader*)InitrdStartAddress;

    while(file_entry != NULL && file_entry->filename[0] != 0) {

        if(strcmp(file_entry->filename, file) == 0) {
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

    /*
        uint64_t fb_addr = R0_GetPhysicalAddress(b_info.FramebufferAddress);
        uint64_t fb_len = b_info.FramebufferPitch * b_info.FramebufferHeight;

        fb_addr = R0_MemoryMap(GetProperty(CardinalProperty_PID, 0),
                               fb_addr,
                               0,
                               fb_len,
                               CachingModeWriteThrough,
                               MemoryAllocationType_MMap,
                               MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_User | MemoryAllocationFlags_Present);

        void *boot_screen_loc = NULL;
        uint64_t boot_screen_size = 0;

        GetFile("boot_screen.data",
                       &boot_screen_loc,
                       &boot_screen_size);

        memset(fb_addr, 0xff, fb_len);

        if(boot_screen_loc != NULL) {

            uint32_t * fb = (uint32_t*)fb_addr;
            uint8_t * bs = (uint8_t*)boot_screen_loc;

            uint32_t red_mask = ((-1 >> (32 - b_info.FramebufferRedMaskSize)) << b_info.FramebufferRedFieldPosition);
            uint32_t green_mask = ((-1 >> (32 - b_info.FramebufferGreenMaskSize)) << b_info.FramebufferGreenFieldPosition);
            uint32_t blue_mask = ((-1 >> (32 - b_info.FramebufferBlueMaskSize)) << b_info.FramebufferBlueFieldPosition);

            for(int i = 0; i < b_info.FramebufferHeight; i++) {
                for(int j = 0; j < b_info.FramebufferWidth; j++) {

                    fb[j] = ((uint32_t)*bs << b_info.FramebufferRedFieldPosition) | ((uint32_t)*(bs + 1) << b_info.FramebufferGreenFieldPosition) | ((uint32_t)*(bs + 2) << b_info.FramebufferBlueFieldPosition);

                    //*(uint32_t*)(fb + j * 4) = 0;
                    //fb[j * b_info.FramebufferBPP/8 + b_info.FramebufferRedFieldPosition / 8] = *(bs);
                    //fb[j * b_info.FramebufferBPP/8 + b_info.FramebufferGreenFieldPosition / 8] = *(bs+1);
                    //fb[j * b_info.FramebufferBPP/8 + b_info.FramebufferBlueFieldPosition / 8] = *(bs+2);

                    bs += 3;
                }
                fb += b_info.FramebufferPitch * 8/b_info.FramebufferBPP;
            }
        }
        */


}
