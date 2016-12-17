#include <cardinal/cardinal_types.h>
#include <cardinal/bootinfo.h>
#include <cardinal/process.h>
#include <cardinal/memory.h>
#include <cardinal/shared_memory.h>
#include <cardinal/syscall_property.h>
#include <cardinal/namespace/server.h>
#include <fileserver/fileserver.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fileserver.h"

uint32_t *fb;
int w, h, p, bpp, r_off, r_len, g_off, g_len, b_off, b_len, a_off, a_len;


int main() {

    CardinalBootInfo b_info;
    if(R0_GetBootInfo(&b_info) != 0)
        return -1;

    uint64_t fb_addr = 0;
    R01_GetPhysicalAddress(GetCurrentProcessUID(), b_info.FramebufferAddress, &fb_addr);

    uint64_t fb_len = b_info.FramebufferPitch * b_info.FramebufferHeight;

    R0_AllocateSharedMemory(fb_len,
                            CachingModeWriteThrough,
                            MemoryAllocationType_MMap,
                            MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_User | MemoryAllocationFlags_Present,
                            fb_addr,
                            &fb_addr);


    uint64_t key = 0;
    GetSharedMemoryKey(fb_addr,
                       fb_len,
                       CachingModeWriteThrough,
                       MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Write | MemoryAllocationFlags_User | MemoryAllocationFlags_Present,
                       &key);

    w = b_info.FramebufferWidth;
    h = b_info.FramebufferHeight;
    p = b_info.FramebufferPitch;

    uint32_t fbuf_len = p * h;

    fb = (uint32_t*)fb_addr;
    for(uint32_t y = 0; y < h; y++) {
        for(uint32_t x = 0; x < p/sizeof(uint32_t); x++) {
            fb[y * p/sizeof(uint32_t) + x] = (uint32_t)0x0000ffff;
        }
    }

    uint32_t op_key = 0;
    uint64_t op_error = 0;
    RegisterNamespace("display", &op_key);
    while(!IsNamespaceRequestReady(op_key, &op_error));


    start_server();
    //Obtain boot information
    //Map the framebuffer into memory
    //Monitor for an access request
    //Grant access to a single application
    //Wait for the application to die
    //Back to monitoring
}