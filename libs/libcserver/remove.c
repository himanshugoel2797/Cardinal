#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleRemoveRequest(Message *m, int (*remove_c)(char *file)) {
    //Resolve the path
    //Get the FilesystemObject, attempt to delete it
    //If the object couldn't be found, call the handler
    //If the path resolves to a mount point, forward the call, ensuring to fill out the forwarding fields if needed
}