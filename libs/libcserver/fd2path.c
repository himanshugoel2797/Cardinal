#include <stdint.h>

#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleFd2PathRequest(Message *m, int (*fd2path)(uint64_t fd, char *path, size_t *path_len))
{

}