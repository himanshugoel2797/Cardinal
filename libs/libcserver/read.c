#include <stdint.h>
#include <stdlib.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleReadRequest(Message *m, int (*read)(uint64_t fd, void *buf, int64_t offset, uint64_t whence, size_t cnt)) {
    struct ReadRequest *read_req = (struct ReadRequest*)m;

    //Error out if the request read is too large
    if(read_req->read_size >= MAX_BUF_LEN) {
        struct ReadResponse read_resp;
        FILL_RESPONSE(&read_resp, read_req)
        read_resp.m.Size = sizeof(struct ReadResponse);
        read_resp.msg_type = CARDINAL_MSG_TYPE_READRESPONSE;
        read_resp.code = -1;

        Message *m = (Message*)&read_resp;
        PostIPCMessages(&m, 1);
        return;
    }

    struct ReadResponse *read_resp = malloc(read_req->read_size + sizeof(struct ReadResponse));
    FILL_RESPONSE(read_resp, read_req)
    read_resp->m.Size = sizeof(struct ReadResponse) + read_req->read_size;
    read_resp->msg_type = CARDINAL_MSG_TYPE_READRESPONSE;

    uint64_t fd = read_req->fd;
    int flags = 0;
    int mode = 0;
    uint64_t hash = 0;
    FileSystemObject *fs_obj = NULL;

    if(!GetFileDescriptor(fd, &flags, &mode, &hash, &fs_obj)) {

        read_resp->code = -1;

        if(read != NULL)
        	read_resp->code = read(fd, read_resp->data, read_req->offset, read_req->whence, read_req->read_size);

        PostIPCMessages((Message**)&read_resp, 1);
        return;
    }

    if(fs_obj->ObjectType == FileSystemObjectType_File) {
        read_resp->code = fs_obj->handlers->read(fs_obj, fd, read_resp->data, read_req->offset, read_req->whence, read_req->read_size);
    } else {
        read_resp->code = -1;
    }

    PostIPCMessages((Message**)&read_resp, 1);
    free(read_resp);
    return;
}