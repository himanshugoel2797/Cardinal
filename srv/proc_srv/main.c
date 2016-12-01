#include "server.h"
#include "common.h"

#include <cardinal/ipc.h>
#include <errno.h>
#include <cardinal/process.h>

void
HandleSystemMessages(Message *m){

}

typedef struct{
    Message m;
    ProcessServerMessageType MsgType;
} MsgHeader;

int
checkGroup(uint64_t expectedGroup, UID dst, uint64_t msgID) {
    uint64_t gid = 3;
    if(GetProcessGroupID(dst, &gid) != 0)
        return 0;

    if(gid > expectedGroup)
    {
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
    while(1) {

        CREATE_NEW_MESSAGE_PTR(msg);
        while(!GetIPCMessage(msg));

        if(msg->MsgType == CardinalMsgType_Request){

            MsgHeader *msg_h = (MsgHeader*)msg;

            switch(msg_h->MsgType) {
                case ProcessServerMessageType_R0_CreateProcessNoShmemRequest:
                {
                    if(checkGroup(0, msg->SourcePID, msg->MsgType))
                        create_process_noshmem_handler(msg);
                }
                break;
                case ProcessServerMessageType_R0_ProcessExistenceNotification:
                {
                    if(checkGroup(0, msg->SourcePID, msg->MsgType))
                        existence_notification_handler(msg);
                }
                break;
                case ProcessServerMessageType_CreateProcessRequest:
                {
                    create_process_handler(msg);
                }
            }
        }else
            HandleSystemMessages(msg);
    }
}