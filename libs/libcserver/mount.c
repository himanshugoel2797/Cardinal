#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleMountRequest(Message *m, int (*MountHandler)(const char*, UID)) {
    struct MountRequest *mnt_req = (struct MountRequest*)m;

    //Add the mount point if the path is valid
}
