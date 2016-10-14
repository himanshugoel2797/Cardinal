#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include <cardinal/ipc.h>
#include <cardinal/file_server.h>

#include "cserver.h"

#include "mount_db.h"
#include "interface_provider.h"
#include "file_request_handlers.h"

int cserver_main(uint64_t destCode,
                 int (*Mounted)(const char *parent),
                 struct cserver_handlers *handlers) {

    if(destCode != 0 && !RegisterSpecialDestination(destCode))
        return -1;

    InitializeDB();
    InitializeInterface();


    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 100;

    Message *m = malloc(MAX_MESSAGE_SIZE + 1);

    char *tmp_m = (char*)m;
    tmp_m[MAX_MESSAGE_SIZE] = 0;	//Force null termination

    while(true) {
        while(!GetIPCMessageFrom(m, 0, 0))
            nanosleep(&t, NULL);

        struct OpenRequest * test_req = (struct OpenRequest*) m;

        switch(test_req->msg_type) {
        case CARDINAL_MSG_TYPE_OPENREQUEST: {
            HandleOpenRequest(m, handlers->open);
        }
        break;
        case CARDINAL_MSG_TYPE_CLOSEREQUEST: {
            HandleCloseRequest(m, handlers->close);
        }
        break;
        case CARDINAL_MSG_TYPE_READREQUEST: {
            HandleReadRequest(m, handlers->read);
        }
        break;
        case CARDINAL_MSG_TYPE_WRITEREQUEST: {
            HandleWriteRequest(m, handlers->write);
        }
        break;
        case CARDINAL_MSG_TYPE_MOUNTREQUEST: {
            HandleMountRequest(m, handlers->MountHandler);
        }
        break;
        case CARDINAL_MSG_TYPE_DIRENTRYREQUEST: {
            HandleDirentryRequest(m, handlers->direntry);
        }
        break;
        case CARDINAL_MSG_TYPE_LINKREQUEST: {
            HandleLinkRequest(m, handlers->link);
        }
        break;
        case CARDINAL_MSG_TYPE_UNLINKREQUEST: {
            HandleUnlinkRequest(m, handlers->unlink);
        }
        break;
        case CARDINAL_MSG_TYPE_STATREQUEST: {
            HandleStatRequest(m, handlers->stat);
        }
        break;
        case CARDINAL_MSG_TYPE_FD2PATHREQUEST: {
            HandleFd2PathRequest(m, handlers->fd2path);
        }
        break;
        case CARDINAL_MSG_TYPE_RENAMEREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_MKDIRREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_CHMODREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_TRUNCATEREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_STATFSREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_FLUSHREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_UTIMENSREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_LOCKREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_UNLOCKREQUEST: {

        }
        break;
        case CARDINAL_MSG_TYPE_MOUNTEVENT: {
            struct MountEvent *mnt_event_data = (struct MountEvent*)m;
            SetMountLocation(mnt_event_data->path);

            if(Mounted != NULL) {
                char path[PATH_MAX];
                GetMountLocation(path, PATH_MAX);
                Mounted(path);
            }
        }
        break;
        }

    }

    free(m);
}