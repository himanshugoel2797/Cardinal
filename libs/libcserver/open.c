#include <stdint.h>
#include <stdbool.h>

#include <fcntl.h>
#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleOpenRequest(Message *m, uint64_t (*open_c)(const char *file, int flags, int mode)) {
    struct OpenRequest *open_req = (struct OpenRequest*)m;
    struct OpenResponse response;

    FileSystemObject *fs_obj = ParsePath(open_req->path);
    if(fs_obj == NULL) *(uint8_t*)0 = 1;

    uint64_t fd = -1;

    FILL_RESPONSE(&response, open_req)
    response.m.Size = sizeof(struct OpenResponse);

    bool skip_open_handler = false;

    if(fs_obj != NULL) {
        if(fs_obj->ObjectType == FileSystemObjectType_File) {

            fd = fs_obj->handlers->open(fs_obj, open_req->path, (int)open_req->flags, (int)open_req->mode);
            skip_open_handler = true;

        } else if(fs_obj->ObjectType == FileSystemObjectType_MountPoint) {
            //Direct request to the server handling the mount request
            uint64_t src = open_req->m.SourcePID;

            open_req->m.SourcePID = CARDINAL_IPCDEST_FILESERVER;
            open_req->m.DestinationPID = fs_obj->TargetPID;

            PostIPCMessages((Message**)&open_req, 1);

            struct timespec t;
            t.tv_sec = 0;
            t.tv_nsec = 100;

            //Wait for the server to respond
            while(GetIPCMessageFrom((Message*)&response, fs_obj->TargetPID, open_req->m.MsgID) != 1)
                nanosleep(&t, NULL);

            //Forward the response to the source
            response.m.SourcePID = CARDINAL_IPCDEST_FILESERVER;
            response.m.DestinationPID = src;

            Message *ma = (Message*)&response;
            PostIPCMessages(&ma, 1);

            return;
        } else if(fs_obj->ObjectType == FileSystemObjectType_Directory && open_req->flags & O_DIRECTORY) {
            //Open the directory

            uint64_t dir_hash = HashPath(open_req->path);
            fd = AllocateFileDescriptor(open_req->flags, open_req->mode, dir_hash, fs_obj);

            skip_open_handler = true;
        }
    }

    if(!skip_open_handler && open_c != NULL) {
        fd = open_c(open_req->path, (int)open_req->flags, (int)open_req->mode);
    }

    response.msg_type = CARDINAL_MSG_TYPE_OPENRESPONSE;
    response.fd = fd;
    response.targetPID = CARDINAL_IPCDEST_FILESERVER;

    Message *ma = (Message*)&response;
    PostIPCMessages(&ma, 1);
    return;
}