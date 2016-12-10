#include <cardinal/cardinal_types.h>
#include <cardinal/bootinfo.h>
#include <cardinal/process.h>
#include <cardinal/memory.h>
#include <cardinal/shared_memory.h>
#include <cardinal/syscall_property.h>

#include <cardinal/namespace/server.h>

#include <libfileserver/fileserver.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t *fb;
uint32_t w,h,p,fbuf_len;

static UID write_pid = 0;
static char *fbuf_info;
static int fbuf_info_len;


void
UnknMessage(Message *m) {
    __asm__("hlt");
}

int
fbuf_open(const char *path,
          int flags,
          uint64_t mode,
          uint8_t* key,
          UID pid,
          uint64_t* fd) {

    if(strcmp(path, ":framebuffer0") == 0) {
        if(flags & FileSystemOpFlag_Write && write_pid == 0)
            write_pid = pid;
        else if(flags & FileSystemOpFlag_Write && write_pid != 0) {
            *fd = write_pid;
            return -EINUSE;
        }

        *fd = 2;

    } else if(strcmp(path, ":info0") == 0) {
        if(flags & FileSystemOpFlag_Write)
            return -ERDONLY;

        *fd = 1;
    } else
        return -EINVAL;
    return 0;
}

int
fbuf_read(uint64_t fd,
          uint64_t offset,
          void *dst,
          uint64_t len,
          UID pid) {
    if(fd == 1) {

        if(offset >= fbuf_info_len)
            return -EEOF;

        if(offset + len > fbuf_info_len)
            len = fbuf_info_len - offset;

        memcpy(dst, fbuf_info + offset, len);
        return (int)len;
    } else if(fd == 2) {

        if(offset >= fbuf_len)
            return -EEOF;

        if(offset + len > fbuf_len)
            len = fbuf_len - offset;

        memcpy(dst, (uint8_t*)fb + offset, len);
        return (int)len;

    } else
        return -EINVAL;
}

int
fbuf_write(uint64_t fd,
           uint64_t offset,
           void *src,
           uint64_t len,
           UID pid) {
    if(fd == 2 && pid == write_pid) {

        if(offset == 0 && len >= fbuf_len) {

            uint64_t *fbufA = fb;
            uint64_t *fbufB = src;

            for (uint32_t a = 0; a < fbuf_len; a+=0x80)
            {
                {
                    __asm__ volatile ("movdqa (%%rbx), %%xmm0\n\t"
                              "movdqa 0x10(%%rbx), %%xmm1\n\t"
                              "movdqa 0x20(%%rbx), %%xmm2\n\t"
                              "movdqa 0x30(%%rbx), %%xmm3\n\t"
                              "movdqa 0x40(%%rbx), %%xmm4\n\t"
                              "movdqa 0x50(%%rbx), %%xmm5\n\t"
                              "movdqa 0x60(%%rbx), %%xmm6\n\t"
                              "movdqa 0x70(%%rbx), %%xmm7\n\t"
                              "shufps $0xE4, %%xmm0,  %%xmm0\n\t"
                              "shufps $0xE4, %%xmm1,  %%xmm1\n\t"
                              "shufps $0xE4, %%xmm2,  %%xmm2\n\t"
                              "shufps $0xE4, %%xmm3,  %%xmm3\n\t"
                              "shufps $0xE4, %%xmm4,  %%xmm4\n\t"
                              "shufps $0xE4, %%xmm5,  %%xmm5\n\t"
                              "shufps $0xE4, %%xmm6,  %%xmm6\n\t"
                              "shufps $0xE4, %%xmm7,  %%xmm7\n\t"
                              "movntdq %%xmm0, (%%rax)\n\t"
                              "movntdq %%xmm1, 0x10(%%rax)\n\t"
                              "movntdq %%xmm2, 0x20(%%rax)\n\t"
                              "movntdq %%xmm3, 0x30(%%rax)\n\t"
                              "movntdq %%xmm4, 0x40(%%rax)\n\t"
                              "movntdq %%xmm5, 0x50(%%rax)\n\t"
                              "movntdq %%xmm6, 0x60(%%rax)\n\t"
                              "movntdq %%xmm7, 0x70(%%rax)\n\t"
                              :: "a" (fbufA), "b" (fbufB) : "%xmm0","%xmm1","%xmm2","%xmm3","%xmm4","%xmm5","%xmm6","%xmm7");
                }

                fbufB+=0x80/sizeof(uint64_t);
                fbufA+=0x80/sizeof(uint64_t);
            }
            return fbuf_len;
        }

        if(offset >= fbuf_len)
            return -EEOF;

        if(offset + len > fbuf_len)
            len = fbuf_len - offset;

        memcpy((uint8_t*)fb + offset, src, len);
        return (int)len;

    } else
        return -EINVAL;
}

void fbuf_close(uint64_t fd,
                UID pid) {
    if(fd == 2 && pid == write_pid) {
        write_pid = 0;
    }
}

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

    fbuf_info_len = snprintf(NULL, 0, "Width:%d Height:%d Pitch:%d", w, h, p);
    fbuf_info = malloc(fbuf_info_len + 1);
    sprintf(fbuf_info, "Width:%d Height:%d Pitch:%d", w, h, p);

    fbuf_len = p * h;

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

    FileServerHandlers handlers;
    handlers.open = fbuf_open;
    handlers.read = fbuf_read;
    handlers.write = fbuf_write;
    handlers.close = fbuf_close;
    handlers.remove = NULL;
    handlers.rename = NULL;
    handlers.sync = NULL;
    Server_Start(&handlers, NULL);


    while(1);
    //Obtain boot information
    //Map the framebuffer into memory
    //Monitor for an access request
    //Grant access to a single application
    //Wait for the application to die
    //Back to monitoring
}