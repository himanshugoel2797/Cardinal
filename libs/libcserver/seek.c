#include <stdint.h>
#include <stdlib.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleSeekRequest(Message *m, int (*seek)(FileSystemObject *handlers, uint64_t fd, int64_t offset, int whence)) {

}