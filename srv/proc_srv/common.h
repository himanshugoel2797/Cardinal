#ifndef _PROC_SRV_COMMON_H_
#define _PROC_SRV_COMMON_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include "server.h"


void 
create_process_noshmem_handler(Message *m);

void
create_process_handler(Message *m);

void
existence_notification_handler(Message *m);


#endif