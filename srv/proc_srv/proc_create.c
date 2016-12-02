#include "common.h"
#include "proc_db.h"
#include "server.h"

#include <cardinal/shared_memory.h>

void
send_existence_notification(UID pid) {
	ProcessServerNotificationType_Notification note;
	note.m.MsgID = RequestMessageID();
	note.m.MsgType = CardinalMsgType_Notification;
	note.MsgType = ProcessServerNotificationType_ProcessCreated;
	note.pid = pid;

	//Post this message to the various servers that need this info.
	Message *m = (Message*)&note;
	PostIPCMessages(MEMORY_SRV_PID, &m, 1);
}

void
create_process_handler(Message *m) {

}

void
existence_notification_handler(Message *m) {
    ProcessServer_R0NotifyProcessExistence *msg = (ProcessServer_R0NotifyProcessExistence*)m;

    char *prog_name = NULL;
    UserSharedMemoryData data;

    if(msg->process_name_key != 0) {
    	if(ApplySharedMemoryKey(msg->process_name_key, &data) != 0) {
    		//TODO send error response
    	}

    	prog_name = data.VirtualAddress;
    }

	ProcDB_AddProcess(msg->pid, prog_name);

	if(prog_name != NULL) {
		Unmap(data.VirtualAddress,
			  data.Length);
	}

	send_existence_notification(msg->pid);
}