#ifndef _MEM_SRV_COMMON_H_
#define _MEM_SRV_COMMON_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include "server.h"


void 
mmap_handler(Message *m);

void
grant_create_handler(Message *m);

void
grant_request_handler(Message *m);

#endif