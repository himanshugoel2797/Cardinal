#include <cardinal/cardinal_types.h>
#include <cardinal/file_server.h>
#include <cardinal/namespace/server.h>

#include <fileserver/fileserver.h>
#include <list/list.h>

#include <stdlib.h>
#include <string.h>

#include "initrd.h"

typedef struct {
	void *loc;
	uint64_t len;
	UID pid;
    uint64_t fd;
} FD_Entry;

static List *fds;
static uint64_t fd_cnt = 0;

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

	if(flags & (FileSystemOpFlag_Create | FileSystemOpFlag_Append | FileSystemOpFlag_Locked | FileSystemOpFlag_Exclusive | FileSystemOpFlag_Directory | FileSystemOpFlag_Write))
		return -EINVAL;

	void *loc = NULL;
	size_t size = 0;

	if(path[0] == ':' && GetFile(path + 1, &loc, &size)) {

		FD_Entry *fd_e = malloc(sizeof(FD_Entry));
		fd_e->loc = loc;
		fd_e->len = size;
		fd_e->pid = pid;
        fd_e->fd = ++fd_cnt;

		List_AddEntry(fds, fd_e);

        *fd = fd_e->fd;
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

    if(fd < fd_cnt) {

        for(uint64_t i = 0; i < List_Length(fds); i++) {
            FD_Entry *fd_e = (FD_Entry*)List_EntryAt(fds, i);

            if(fd_e->fd == fd && fd_e->pid == pid) {

                if(offset > fd_e->len)
                    return -EEOF;

                if(offset + len > fd_e->len) {
                    len = fd_e->len - offset;
                }

                memcpy(dst, fd_e->loc, len);
                return len;
            }
        }

    }
    return -EINVAL;
}

void fbuf_close(uint64_t fd,
                UID pid) {
    
    if(fd < fd_cnt) {
        for(uint64_t i = 0; i < List_Length(fds); i++) {
            FD_Entry *fd_e = (FD_Entry*)List_EntryAt(fds, i);

            if(fd_e->fd == fd && fd_e->pid == pid) {
                List_Remove(fds, i);
                free(fd_e);
                break;
            }
        }
    }

}

int
get_file_properties(const char *file,
                    FileSystemDirectoryEntry *dir,
                    UID pid) {
    
    void *loc = NULL;
    size_t size = 0;

    if(file[0] == ':' && GetFile(file + 1, &loc, &size)) {

        dir->Length = size;
        return 0;

    } else
        return -EINVAL;
}


void
start_server(void) {

    FileServerHandlers handlers;
    handlers.open = fbuf_open;
    handlers.read = fbuf_read;
    handlers.write = NULL;
    handlers.close = fbuf_close;
    handlers.remove = NULL;
    handlers.rename = NULL;
    handlers.sync = NULL;
    handlers.get_file_properties = get_file_properties;
    Server_Start(&handlers, NULL);
}

int main() {

	ImportInitrd();

	//Serve the initrd's files, read only
	fds = List_Create();

    uint32_t op_key = 0;
    uint64_t op_error = 0;
    RegisterNamespace("initrd", &op_key);
    while(!IsNamespaceRequestReady(op_key, &op_error));

    start_server();

}