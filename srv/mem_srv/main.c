#include "server.h"
#include "common.h"
#include "mem_db.h"

#include <cardinal/ipc.h>
#include <procserver/server.h>

//TODO implement mmap and grant support for now, to allow musl to initialize, namespace_dir relies on it.
//namespace_dir implements a simple list of namespaces and associated pids, it can receive process death notifications from the process server.
//proc_srv provides process creation and management services, initially only allow R0 processes to use memory based executable loading.
//elf_srv loads elf files, it can be called by proc_srv only.
//sys_init acts as the parent for the rest of the system.

//TODO enable keys once again, but first determine exactly how they are to be used.
typedef struct {
    Message m;
    uint32_t MsgType;
} MsgHeader;

void
HandleSystemMessages(Message *m) {

    MsgHeader *msg_h = (MsgHeader*)m;

    switch(m->MsgType) {
    case CardinalMsgType_Notification:
        //Check if it is a process creation or process deletion notification
    {
        if(m->SourcePID == PROCESS_SRV_PID) {
            switch(msg_h->MsgType) {
            case ProcessServerNotificationType_ProcessCreated: {
                ProcessServerNotificationType_Notification *n = (ProcessServerNotificationType_Notification*)msg_h;
                MemDB_AddProcess(n->pid);
            }
            break;
            case ProcessServerNotificationType_ProcessExited: {
                ProcessServerNotificationType_Notification *n = (ProcessServerNotificationType_Notification*)msg_h;
                MemDB_FreeProcess(n->pid);
            }
            break;
            }
        }
    }
        //Update the mount database appropriately.
    break;
    default:
        return;
    }
}


int main() {

    MemDB_Initialize();
    while(1) {
        CREATE_NEW_MESSAGE_PTR(msg);
        POLL_MESSAGE(msg);

        if(msg->MsgType == CardinalMsgType_Request) {

            MsgHeader *msg_h = (MsgHeader*)msg;

            switch(msg_h->MsgType) {
            case MemoryServerMessageType_MMapRequest: {
                mmap_handler(msg);
            }
            break;
            case MemoryServerMessageType_GrantCreationRequest: {
                grant_create_handler(msg);
            }
            break;
            case MemoryServerMessageType_GrantRequest: {
                grant_request_handler(msg);
            }
            }


        } else
            HandleSystemMessages(msg);
    }
}