#include <cardinal/io/server.h>
#include <cardinal/namespace/server.h>
#include <string.h>
#include <stdio.h>

#include "display.h"

static UID display_pid;
static int display_cnt;

int
Display_Initialize(void) {

    uint64_t error = 0;
    uint32_t tmp_key = 0;
    RetrieveNamespace("display", &tmp_key);
    while(!IsNamespaceRetrieved(tmp_key, &display_pid, &error));

    return (error != 0);
}

int
Display_GetDisplayCount(void) {
    uint64_t fd = 0;
    uint8_t key[KEY_BYTES];

    //Read the root directory and count the number of files that match displayXXX
    uint64_t buf_len = sizeof(FileSystemDirectoryData) + 128 * sizeof(FileSystemDirectoryEntry);
    uint64_t vAddr = 0;
    uint64_t read_key = 0, write_key = 0;
    IO_AllocateBuffer(&buf_len,
                      &vAddr,
                      &read_key,
                      &write_key);

    IO_Open(":", FileSystemOpFlag_Read | FileSystemOpFlag_Directory, 0, key, display_pid, &fd);
    IO_Read(fd, 0, read_key, buf_len, display_pid);
    IO_Close(fd, display_pid);

    //TODO make this actually support reading all the directories

    display_cnt = 0;

    FileSystemDirectoryData *dirs = (FileSystemDirectoryData*)vAddr;
    for(int i = 0; i < dirs->hdr.EntryCount; i++) {
        if(strncmp(dirs->Entries[i].Name, "display", strlen("display")) == 0) {
            display_cnt++;
        }
    }


    IO_FreeBuffer(vAddr, buf_len, read_key, write_key);
    return display_cnt;
}

int
Display_GetInfo(int display_index,
                DisplayInfo *disp_info) {

    uint64_t fd = 0;
    uint8_t key[KEY_BYTES];

    uint64_t buf_len = 1024;
    uint64_t vAddr = 0;
    uint64_t read_key = 0, write_key = 0;
    IO_AllocateBuffer(&buf_len,
                      &vAddr,
                      &read_key,
                      &write_key);

    char filename[256];
    snprintf(filename, 256, ":info%04d", display_index);

    IO_Open(filename, FileSystemOpFlag_Read, 0, key, display_pid, &fd);
    IO_Read(fd, 0, read_key, buf_len, display_pid);
    IO_Close(fd, display_pid);

    char name[NAME_MAX];

    char *infoStr = (char*)vAddr;
    sscanf(infoStr, DISPLAY_INFO_STRING,&name,
           &disp_info->width,
           &disp_info->height,
           &disp_info->pitch,
           &disp_info->bpp,
           &disp_info->R_offset,
           &disp_info->R_length,
           &disp_info->G_offset,
           &disp_info->G_length,
           &disp_info->B_offset,
           &disp_info->B_length,
           &disp_info->A_offset,
           &disp_info->A_length);

    IO_FreeBuffer(vAddr, buf_len, read_key, write_key);
    return 0;
}

int
Display_GetDisplayImage(int display_index,
                        uint64_t key,
                        uint64_t len) {
    //Read the display
    uint8_t sec_key[KEY_BYTES];
    uint64_t fd = 0;

    char filename[256];
    snprintf(filename, 256, ":display%04d", display_index);

    IO_Open(filename, FileSystemOpFlag_Read, 0, sec_key, display_pid, &fd);
    IO_Read(fd, 0, key, len, display_pid);
    IO_Close(fd, display_pid);
}

uint64_t
Display_TryLockDisplay(int display_index) {
    //Try to open the display for writing, thus getting exclusive access

    uint8_t sec_key[KEY_BYTES];
    uint64_t fd = 0;

    char filename[256];
    snprintf(filename, 256, ":display%04d", display_index);

    IO_Open(filename, FileSystemOpFlag_Write, 0, sec_key, display_pid, &fd);

    return fd;
}

int
Display_UnlockDisplay(uint64_t display_fd) {
    //Unlock the lock owned by this process on the display
    IO_Close(display_fd, display_pid);
    return 0;
}

void
Display_Update(uint64_t display_fd,
               uint64_t buffer_key,
               uint64_t buffer_len) {
    //Write to the display

    IO_Write(display_fd, 0, buffer_key, buffer_len, display_pid);
}