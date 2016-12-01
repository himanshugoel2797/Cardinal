#include "server.h"
#include "common.h"
#include "mem_db.h"

#include <cardinal/ipc.h>

//TODO implement mmap and grant support for now, to allow musl to initialize, namespace_dir relies on it.
//namespace_dir implements a simple list of namespaces and associated pids, it can receive process death notifications from the process server.
//proc_srv provides process creation and management services, initially only allow R0 processes to use memory based executable loading.
//elf_srv loads elf files, it can be called by proc_srv only.
//sys_init acts as the parent for the rest of the system.

//TODO enable keys once again, but first determine exactly how they are to be used.

void
HandleSystemMessages(Message *m){
	switch(m->MsgType){
		case CardinalMsgType_Notification:
			//Check if it is a process creation or process deletion notification
			//Update the mount database appropriately.
		break;
		default:
		return;
	}
}

typedef struct{
	Message m;
	MemoryServerMessageType MsgType;
} MsgHeader;

int main() {

	MemDB_Initialize();
	while(1) {
		CREATE_NEW_MESSAGE_PTR(msg);
		while(!GetIPCMessage(msg));

		if(msg->MsgType == CardinalMsgType_Request){

			MsgHeader *msg_h = (MsgHeader*)msg;

			switch(msg_h->MsgType) {
				case MemoryServerMessageType_MMapRequest:
				{
					mmap_handler(msg);
				}
				break;
				case MemoryServerMessageType_GrantCreationRequest:
				{
					grant_create_handler(msg);
				}
				break;
				case MemoryServerMessageType_GrantRequest:
				{
					grant_request_handler(msg);
				}
			}


		}else
			HandleSystemMessages(msg);
	}
}