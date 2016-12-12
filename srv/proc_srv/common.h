#ifndef _PROC_SRV_COMMON_H_
#define _PROC_SRV_COMMON_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/proc/server.h>
#include <cardinal/ipc.h>


void
proc_initialize(void);

void
subscribe_creation(Message *m);

void
subscribe_exit(Message *m);

void
exit_process(Message *m);

void
create_process_handler(Message *m);

void
existence_notification_handler(Message *m);


#endif