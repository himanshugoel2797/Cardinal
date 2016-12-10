#include <cardinal/io/server.h>
#include <cardinal/namespace/server.h>
#include <string.h>
#include <stdio.h>

#include "initrd.h"
#include "png.h"

int main() {
    //First map in the initrd
    ImportInitrd();

    //Use it along with the loaded elf loader to load the initial system processes
    uint8_t key[KEY_BYTES];
    uint64_t fd = 0;

    UID pid = 0;
    uint64_t error = 0;

    uint32_t tmp_key = 0;
    RetrieveNamespace("display", &tmp_key);
    while(!IsNamespaceRetrieved(tmp_key, &pid, &error));

    uint64_t buf_len = 1024;
    uint64_t vAddr = 0;
    uint64_t read_key = 0, write_key = 0;
    IO_AllocateBuffer(&buf_len,
                      &vAddr,
                      &read_key,
                      &write_key);

    memset((void*)vAddr, 0x00, buf_len);

    IO_Open(":info0", FileSystemOpFlag_Read, 0, key, pid, &fd);
    IO_Read(fd, 0, read_key, buf_len, pid);
    IO_Close(fd, pid);

    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t p = 0;
    sscanf((char*)vAddr, "Width:%d Height:%d Pitch:%d", &w, &h, &p);

    IO_FreeBuffer(vAddr, buf_len, read_key, write_key);

    buf_len = h * p;
    IO_AllocateBuffer(&buf_len,
                      &vAddr,
                      &read_key,
                      &write_key);

    memset((void*)vAddr, 0xFF, buf_len);

    void *file_loc = NULL;
    size_t file_size = 0;
    GetFile("wallpaper.png", &file_loc, &file_size);

    int img_w = 0;
    int img_h = 0;
    int img_p = 0;
    int res_len = 0;

    void *result = DecodePNGtoRGBA(file_loc, file_size, &img_w, &img_h, &img_p, &res_len);

    memcpy(vAddr, result, res_len);

    IO_Open(":framebuffer0", FileSystemOpFlag_Write, 0, key, pid, &fd);
    IO_Write(fd, 0, write_key, buf_len, pid);

    //Wait until exit requested on power off
    while(1);
}