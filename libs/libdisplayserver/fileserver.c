/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/cardinal_types.h>
#include <cardinal/bootinfo.h>
#include <cardinal/process.h>
#include <cardinal/memory.h>
#include <cardinal/shared_memory.h>
#include <cardinal/syscall_property.h>

#include <cardinal/namespace/server.h>

#include <fileserver/fileserver.h>
#include <display/display.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern uint32_t *fb;
extern int w, h, p, bpp, r_off, r_len, g_off, g_len, b_off, b_len, a_off, a_len;
UID write_pid = 0;

FileSystemDirectoryData *dir_data;
char *display0000_info;

static uint64_t dir_data_len = 0;
static uint64_t display0000_info_len = 0;

#define ROOT_FD 1
#define FRAMEBUFFER0_FD 2
#define FRAMEBUFFER0_INFO_FD 3

#define DISPLAY0000_STR "display0000"
#define DISPLAY0000_INFO_STR "info0000"

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

    if(flags & (FileSystemOpFlag_Create | FileSystemOpFlag_Append | FileSystemOpFlag_Locked))
        return -EINVAL;

    if(strcmp(path, ":") == 0) {

        if(flags & (FileSystemOpFlag_Write | FileSystemOpFlag_Exclusive))
            return -EINVAL;

        if(!(flags & FileSystemOpFlag_Directory))
            return -EISDIR;

        *fd = ROOT_FD;

    } else if(strcmp(path, ":"DISPLAY0000_STR) == 0) {

        if(flags & FileSystemOpFlag_Directory)
            return -EISFILE;

        if(flags & FileSystemOpFlag_Write && write_pid == 0)
            write_pid = pid;
        else if(flags & FileSystemOpFlag_Write && write_pid != 0) {
            *fd = write_pid;
            return -EINUSE;
        }

        *fd = FRAMEBUFFER0_FD;

    } else if(strcmp(path, ":"DISPLAY0000_INFO_STR) == 0) {

        if(flags & FileSystemOpFlag_Directory)
            return -EISFILE;

        if(flags & (FileSystemOpFlag_Write | FileSystemOpFlag_Exclusive))
            return -EINVAL;

        *fd = FRAMEBUFFER0_INFO_FD;

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
    if(fd == ROOT_FD) {

        if(offset >= dir_data_len)
            return -EEOF;

        if(offset + len > dir_data_len)
            len = dir_data_len - offset;

        memcpy(dst, (uint8_t*)dir_data + offset, len);
        return (int)len;

    } else if(fd == FRAMEBUFFER0_FD) {

        uint32_t fbuf_len = p * h;
        if(offset >= fbuf_len)
            return -EEOF;

        if(offset + len > fbuf_len)
            len = fbuf_len - offset;

        memcpy(dst, (uint8_t*)fb + offset, len);
        return (int)len;

    } else if(fd == FRAMEBUFFER0_INFO_FD ) {

        if(offset >= display0000_info_len)
            return -EEOF;

        if(offset + len > display0000_info_len)
            len = display0000_info_len - offset;

        memcpy(dst, (uint8_t*)display0000_info + offset, len);
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
    if(fd == FRAMEBUFFER0_FD && pid == write_pid) {
        uint64_t fbuf_len = p * h;


        if(offset == 0 && len >= fbuf_len) {

            uint64_t *fbufA = (uint64_t*)fb;
            uint64_t *fbufB = (uint64_t*)src;

            for (uint32_t a = 0; a < fbuf_len; a+=0x80) {
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
    if(fd == FRAMEBUFFER0_FD && pid == write_pid) {
        write_pid = 0;
    }
}

int
fbuf_get_file_properties(const char *file,
                         FileSystemDirectoryEntry *dir,
                         UID pid) {
    if(strcmp(file, DISPLAY0000_STR) == 0) {
        memcpy(dir, &dir_data->Entries[0], sizeof(FileSystemDirectoryEntry));
        return 0;
    } else
        return -EINVAL;
}

void
start_server(int max_disp_count) {

    dir_data_len = sizeof(FileSystemDirectoryData) + sizeof(FileSystemDirectoryEntry) * 1;
    dir_data = malloc(dir_data_len);

    dir_data->hdr.EntryCount = 1;
    strcpy(dir_data->Entries[0].Name, DISPLAY0000_STR);
    dir_data->Entries[0].FileType = FileTypeFlag_DataFile;
    dir_data->Entries[0].AccessMode = FileSystemOpFlag_Exclusive;
    dir_data->Entries[0].Length = p * h;
    memset(dir_data->Entries[0].ReadKID, 0, KID_SIZE_BYTES);
    memset(dir_data->Entries[0].WriteKID, 0, KID_SIZE_BYTES);
    memset(dir_data->Entries[0].ExecuteKID, 0, KID_SIZE_BYTES);

    display0000_info_len = snprintf(NULL, 0, DISPLAY_INFO_STRING, "VESA", w, h, p, bpp, r_off, r_len, g_off, g_len, b_off, b_len, a_off, a_len);
    display0000_info = malloc(display0000_info_len + 1);
    sprintf(display0000_info, DISPLAY_INFO_STRING, "VESA", w, h, p, bpp, r_off, r_len, g_off, g_len, b_off, b_len, a_off, a_len);

    FileServerHandlers handlers;
    handlers.open = fbuf_open;
    handlers.read = fbuf_read;
    handlers.write = fbuf_write;
    handlers.close = fbuf_close;
    handlers.remove = NULL;
    handlers.rename = NULL;
    handlers.sync = NULL;
    Server_Start(&handlers, NULL);
}