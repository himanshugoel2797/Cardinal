#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleLinkRequest(Message *m, int (*link)(char *from, char *to)) {

}