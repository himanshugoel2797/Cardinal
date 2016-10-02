#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleDirentryRequest(Message *m, int (*direntry)(FileSystemObject *handlers, uint64_t fd, struct DirentryData *data, size_t data_size)) {

}
