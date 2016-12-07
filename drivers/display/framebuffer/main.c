#include <cardinal/cardinal_types.h>
#include <cardinal/bootinfo.h>
#include <cardinal/process.h>
#include <cardinal/memory.h>
#include <cardinal/syscall_property.h>

int main() {

    CardinalBootInfo b_info;
    if(R0_GetBootInfo(&b_info) != 0)
        return -1;

    uint64_t fb_addr = 0;
    R0_GetPhysicalAddress(GetCurrentProcessUID(), b_info.FramebufferAddress, &fb_addr);

    uint64_t fb_len = b_info.FramebufferPitch * b_info.FramebufferHeight;

    R0_AllocateSharedMemory(fb_len,
                            CachingModeWriteThrough,
                            MemoryAllocationType_MMap,
                            MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_User | MemoryAllocationFlags_Present
                            fb_addr,
                            &fb_addr);

    uint64_t key = 0;
    GetSharedMemoryKey(fb_addr,
                       fb_len,
                       CachingModeWriteThrough,
                       MemoryAllocationFlags_Write | MemoryAllocationFlags_User | MemoryAllocationFlags_Present,
                       &key);

    uint32_t w = b_info.FramebufferWidth;
    uint32_t h = b_info.FramebufferHeight;
    uint32_t p = b_info.FramebufferPitch;

    uint32_t *fb = (uint32_t*)fb_addr;
    for(uint32_t y = 0; y < h; y++) {
        for(uint32_t x = 0; x < p/sizeof(uint32_t); x++) {
            fb[y * p/sizeof(uint32_t) + x] = (uint32_t)-1;
        }
    }

    while(1);
    //Obtain boot information
    //Map the framebuffer into memory
    //Monitor for an access request
    //Grant access to a single application
    //Wait for the application to die
    //Back to monitoring
}