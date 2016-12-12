#include <cardinal/cardinal_types.h>
#include <cardinal/file_server.h>

#include <fileserver/fileserver.h>
#include <list/list.h>

#include <stdlib.h>

#include "initrd.h"

typedef struct {
	void *loc;
	uint64_t len;
	UID pid;
} FD_Entry;

static List *fds;

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

	if(flags & (FileSystemOpFlag_Create | FileSystemOpFlag_Append | FileSystemOpFlag_Locked | FileSystemOpFlag_Exclusive | FileSystemOpFlag_Directory | FilesystemOpFlag_Write))
		return -EINVAL;

	void *loc = NULL;
	size_t size = 0;

	if(GetFile(path, &loc, &size)) {

		FD_Entry *fd = malloc(sizeof(FD_Entry));
		fd->loc = loc;
		fd->len = size;
		fd->pid = pid;

		List_AddEntry(fds, fd);

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

void fbuf_close(uint64_t fd,
                UID pid) {
    if(fd == FRAMEBUFFER0_FD && pid == write_pid) {
        write_pid = 0;
    }
}


void
start_server(void) {

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
    handlers.write = NULL;
    handlers.close = fbuf_close;
    handlers.remove = NULL;
    handlers.rename = NULL;
    handlers.sync = NULL;
    Server_Start(&handlers, NULL);
}

int main() {

	ImportInitrd();

	//Serve the initrd's files, read only
	start_server();

}