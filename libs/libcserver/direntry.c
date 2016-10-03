#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleDirentryRequest(Message *m, int (*direntry)(uint64_t fd, struct DirEntryData *data, size_t data_size)) {

}
