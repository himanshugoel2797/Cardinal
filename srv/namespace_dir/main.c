#include <cardinal/cardinal_types.h>
#include <cardinal/namespace/server.h>
#include <cardinal/proc/server.h>
#include <cardinal/ipc.h>
#include "namespace_dir.h"


typedef struct {
    Message m;
    uint32_t MsgType;
} MsgHeader;

void
send_err_response(UID val, int result, uint32_t msgID, UID dst) {
    CREATE_NEW_MESSAGE_PTR_TYPE(NamespaceRegistrationResponse, resp);
    resp->m.MsgType = CardinalMsgType_Response;
    resp->m.MsgID = msgID;
    resp->pid = val;

    resp->result = 0;

    if(result == -1)
        resp->result = -EINVAL;
    else if(result == -2 || result == -3)
        resp->result = -ENAVAIL;

    PostIPCMessages(dst, (Message**)&resp, 1);
}

void
handle_request(MsgHeader *m) {
    switch(m->MsgType) {
    case NamespaceServerMessageType_Registration: {
        NamespaceRegistrationRequest *req = (NamespaceRegistrationRequest*)m;
        int err_code = NamespaceDir_AddNamespace(req->namespace_name, req->m.SourcePID);
        send_err_response(0, err_code, req->m.MsgID, req->m.SourcePID);
    }
    break;
    case NamespaceServerMessageType_Removal: {
        NamespaceRemovalRequest *req = (NamespaceRemovalRequest*)m;
        int err_code = NamespaceDir_RemoveNamespace(req->namespace_name, req->m.SourcePID);
        send_err_response(0, err_code, req->m.MsgID, req->m.SourcePID);
    }
    break;
    case NamespaceServerMessageType_Retrieval: {
        NamespaceRetrievalRequest *req = (NamespaceRetrievalRequest*)m;
        UID pid = 0;
        int err_code = NamespaceDir_GetNamespacePID(req->namespace_name, &pid);
        send_err_response(pid, err_code, req->m.MsgID, req->m.SourcePID);
    }
    break;
    }
}

void
handle_notification(MsgHeader *m) {

    if(m->m.SourcePID == PROCESS_SRV_PID) {
        switch(m->MsgType) {
        case ProcessServerNotificationType_ProcessCreated: {

        }
        break;
        case ProcessServerNotificationType_ProcessExited: {
            ProcessServerNotificationType_Notification *n = (ProcessServerNotificationType_Notification*)m;
            NamespaceDir_RemoveProcess(n->pid);
        }
        break;
        }
    }

}

int main() {
    NamespaceDir_Initialize();

    CREATE_NEW_MESSAGE_PTR(msg);
    msg->MsgType = CardinalMsgType_Notification;
    msg->MsgID = 0;
    PostIPCMessages(2 /*userboot PID*/, &msg, 1);

    uint32_t op_key = 0;
    uint64_t op_error = 0;
    RegisterNamespace("namespaces", &op_key);


    while(1) {
        CREATE_NEW_MESSAGE_PTR(m);
        POLL_MESSAGE(m);


        MsgHeader *hdr = (MsgHeader*)m;

        switch(m->MsgType) {
        case CardinalMsgType_Request:
            handle_request(hdr);
            break;
        case CardinalMsgType_Notification:
            handle_notification(hdr);
            break;
        default:
            //TODO handle other errors
            break;
        }
    }
}