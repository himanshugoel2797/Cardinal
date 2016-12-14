#include <cardinal/proc/server.h>
#include <cardinal/process.h>
#include <cardinal/ipc.h>
#include <cardinal/driver_utils.h>
#include "common.h"
#include "proc_db.h"

void
HandleSystemMessages(Message *m) {

}

typedef struct {
    Message m;
    ProcessServerMessageType MsgType;
} MsgHeader;

int
checkGroup(uint64_t expectedGroup, UID dst, uint64_t msgID) {
    uint64_t gid = 3;
    if(GetProcessGroupID(dst, &gid) != 0)
        return 0;

    if(gid > expectedGroup) {
        CREATE_NEW_MESSAGE_PTR(msg);
        struct ErrorMessage *err_msg = (struct ErrorMessage*)msg;
        err_msg->m.MsgID = msgID;
        err_msg->m.MsgType = CardinalMsgType_Error;
        err_msg->code = -EPERM;

        PostIPCMessages(dst, &msg, 1);
        return 0;
    }
    return 1;
}

int main() {
    ProcDB_Initialize();
    proc_initialize();

    CREATE_NEW_MESSAGE_PTR(msg);
    msg->MsgType = CardinalMsgType_Notification;
    msg->MsgID = 0;
    PostIPCMessages(2 /*userboot PID*/, &msg, 1);

    while(1) {

        POLL_MESSAGE(msg);

        if(msg->MsgType == CardinalMsgType_Request) {

            MsgHeader *msg_h = (MsgHeader*)msg;

            switch(msg_h->MsgType) {
            case ProcessServerMessageType_CreateProcessRequest: {
                create_process_handler(msg);
            }
            break;
            case ProcessServerMessageType_SubscribeToExitNotification: {
                subscribe_exit(msg);
            }
            break;
            case ProcessServerMessageType_SubscribeToCreateNotification: {
                subscribe_creation(msg);
            }
            break;
            case ProcessServerMessageType_ExitProcess: {
                exit_process(msg);
            }
            break;
            }
        } else if(msg->MsgType == CardinalMsgType_Notification) {
            MsgHeader *msg_h = (MsgHeader*)msg;


            switch(msg_h->MsgType) {
            case ProcessServerMessageType_R0_ProcessExistenceNotification: {
                if(checkGroup(0, msg->SourcePID, msg->MsgType))
                    existence_notification_handler(msg);
            }
            break;
            }
        } else
            HandleSystemMessages(msg);
    }
}