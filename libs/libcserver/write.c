#include <stdint.h>
#include <stdlib.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleWriteRequest(Message *m, int (*write)(FileSystemObject *handlers, uint64_t fd, void *buf, size_t cnt)) {
    struct WriteRequest *write_req = (struct WriteRequest*)m;

    struct WriteResponse write_resp;
    FILL_RESPONSE(&write_resp, write_req)
    write_resp.m.Size = sizeof(struct WriteResponse);
    write_resp.msg_type = CARDINAL_MSG_TYPE_WRITERESPONSE;

    uint64_t fd = write_req->fd;
    int flags = 0;
    int mode = 0;
    uint64_t hash = 0;
    FileSystemObject *fs_obj = NULL;

    if(!GetFileDescriptor(fd, &flags, &mode, &hash, &fs_obj)) {
        write_resp.write_size = -1;

        Message *m2 = (Message*)&write_resp;
        PostIPCMessages((Message**)&m2, 1);
        return;
    }

    if(fs_obj->ObjectType == FileSystemObjectType_File) {
        write_resp.write_size = fs_obj->handlers->write(fs_obj, fd, write_req->buf, write_req->m.Size - sizeof(struct WriteRequest));
    } else {
        write_resp.write_size = -1;
    }

    Message *m2 = (Message*)&write_resp;

    PostIPCMessages((Message**)&m2, 1);
    return;
}