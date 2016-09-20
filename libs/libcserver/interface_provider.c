#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "interface_provider.h"
#include "list.h"
#include "mount_db.h"

static uint64_t mountPath_hash = 0;

uint64_t open_mount(FileSystemObject *handlers, const char *file, int flags, int mode) {
	//return a file descriptor and make sure the path is expected

	if(strcmp(file, "/srv/fs/mount") != 0)
		return -1;

	if(mountPath_hash == 0)
		mountPath_hash = HashPath("/srv/fs/mount");

	return AllocateFileDescriptor(flags, mode, mountPath_hash, handlers);
}

int close_mount(FileSystemObject *handlers, uint64_t fd) {

	FileSystemObject *fs_obj = NULL;

	uint64_t hash = 0;
	if(!GetFileDescriptor(fd, NULL, NULL, &hash, &fs_obj))
		return -1;

	if(hash != mountPath_hash)
		return -1;

	FreeFileDescriptor(fd);
}

int read_mount(FileSystemObject *handlers, uint64_t fd, void *buf, size_t cnt) {

	FileSystemObject *fs_obj = NULL;

	int flags = 0;
	int mode = 0;
	uint64_t hash = 0;
	if(!GetFileDescriptor(fd, &flags, &mode, &hash, &fs_obj))
		return -1;

	if(hash != mountPath_hash)
		return -1;

	return -EBADF;
}

int write_mount(FileSystemObject *handlers, uint64_t fd, void *buf, size_t cnt) {

	FileSystemObject *fs_obj = NULL;

	int flags = 0;
	int mode = 0;
	uint64_t hash = 0;
	if(!GetFileDescriptor(fd, &flags, &mode, &hash, &fs_obj))
		return -1;

	if(hash != mountPath_hash)
		return -1;

	const char *cmd = (const char*)buf;
	const char *cmd_name = "mount";

	for(int i = 0; i < sizeof("mount") - 1; i++) {
		if(cmd[i] != cmd_name[i])
			__asm__("hlt");
	}
        
        return 5;
}

bool
InitializeInterface(void) {
	if(CreateDirectory("/srv") != FileSystemError_None)
		return false;


	if(CreateDirectory("/srv/fs") != FileSystemError_None)
		return false;

	FileHandlers *mnt_handlers = malloc(sizeof(FileHandlers));
	mnt_handlers->open = open_mount;
	mnt_handlers->close = close_mount;
	mnt_handlers->read = read_mount;
	mnt_handlers->write = write_mount;

	if(CreateFile("/srv/fs/mount", mnt_handlers) != FileSystemError_None)
		return false;

	return true;
}