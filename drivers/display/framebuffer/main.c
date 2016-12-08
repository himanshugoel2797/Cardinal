#include <cardinal/cardinal_types.h>
#include <cardinal/bootinfo.h>
#include <cardinal/process.h>
#include <cardinal/memory.h>
#include <cardinal/shared_memory.h>
#include <cardinal/syscall_property.h>
#include <libfileserver/fileserver.h>
#include <namespaceserver/server.h>
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
          uint64_t* fd){

    if(strcmp(path, ":framebuffer") == 0){
        if(flags & FileSystemOpFlag_Write && write_pid == 0)
            write_pid = pid;
        else if(flags & FileSystemOpFlag_Write && write_pid != 0){
            *fd = write_pid;
            return -EINUSE;
        }

        *fd = 2;

    }else if(strcmp(path, ":info") == 0){
        if(flags & FileSystemOpFlag_Write)
            return -ERDONLY;

        *fd = 1;
    }else
        return -EINVAL;
    return 0;
}

int 
fbuf_read(uint64_t fd,
          uint64_t offset,
          void *dst,
          uint64_t len,
          UID pid) {
        __asm__("hlt");
    if(fd == 1) {

        if(offset >= fbuf_info_len)
            return -EEOF;

        if(offset + len > fbuf_info_len)
            len = fbuf_info_len - offset;

        memcpy(dst, fbuf_info + offset, len);
        return (int)len;
    }else if(fd == 2) {

        if(offset >= fbuf_len)
            return -EEOF;

        if(offset + len > fbuf_len)
            len = fbuf_len - offset;

        memcpy(dst, (uint8_t*)fb + offset, len);
        return (int)len;

    }else
        return -EINVAL;
}

int 
fbuf_write(uint64_t fd,
           uint64_t offset,
           void *src,
           uint64_t len,
           UID pid) {
    if(fd == 2 && pid == write_pid) {

        if(offset >= fbuf_len)
            return -EEOF;

        if(offset + len > fbuf_len)
            len = fbuf_len - offset;

        memcpy((uint8_t*)fb + offset, src, len);
        return (int)len;

    }else
        return -EINVAL;    
}

int fbuf_close(uint64_t fd,
               UID pid) {
    if(fd == 2 && pid == write_pid) {
        write_pid = 0;
        return 0;
    }else if(fd == 1)
        return 0;

    return -EINVAL;
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
    RegisterNamespace("disp0", &op_key);
    while(!IsNamespaceRequestReady(op_key, &op_error));

    FileServerHandlers handlers;
    handlers.open = fbuf_open;
    handlers.read = fbuf_read;
    handlers.write = fbuf_write;
    handlers.close = fbuf_close;
    handlers.remove = NULL;
    handlers.get_info = NULL;
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