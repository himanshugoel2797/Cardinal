#include "common.h"
#include "proc_db.h"
#include "server.h"

void
send_existence_notification(UID pid) {

}

void
create_process_handler(Message *m) {

}

void
existence_notification_handler(Message *m) {
	ProcessServer_R0NotifyProcessExistence *msg = (ProcessServer_R0NotifyProcessExistence*)m;


//	ProcDB_AddProcess(msg->pid, )	
}