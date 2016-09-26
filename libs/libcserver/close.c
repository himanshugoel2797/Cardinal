#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleCloseRequest(Message *m, int (*close)(FileSystemObject *handlers, uint64_t fd)) {
	struct CloseRequest *close_req = (struct CloseRequest*)m;

	uint64_t fd = close_req->fd;
	int flags = 0;
	int mode = 0;
	uint64_t hash = 0;
	FileSystemObject *fs_obj = NULL;

	if(!GetFileDescriptor(fd, &flags, &mode, &hash, &fs_obj))
		return;

	if(fs_obj->ObjectType == FileSystemObjectType_File) {
		fs_obj->handlers->close(fs_obj, close_req->fd);
	}

	return;
}