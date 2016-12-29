#include <cardinal/cardinal_types.h>
#include <cardinal/namespace/server.h>
#include <cardinal/root/server.h>
#include <cardinal/proc/server.h>
#include <cardinal/ipc.h>
#include <cardinal/shared_memory.h>
#include <fileserver/fileserver.h>
#include "root_dir.h"


typedef struct {
    Message m;
    uint32_t MsgType;
} MsgHeader;

void
send_err_response(UID val, int result, uint32_t msgID, UID dst) {
    CREATE_NEW_MESSAGE_PTR_TYPE(RootMountResponse, resp);
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
    case RootServerMessageType_Mount: {
        RootMountRequest *req = (RootMountRequest*)m;

        int err_code = 0;

        UserSharedMemoryData shmem_data;
        if(ApplySharedMemoryKey(req->path_key, &shmem_data) != 0) {
            err_code = -EINVAL;
            break;
        }

        err_code = RootDir_AddMount(shmem_data.VirtualAddress, req->m.SourcePID);
        send_err_response(0, err_code, req->m.MsgID, req->m.SourcePID);
        Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
    }
    break;
    case RootServerMessageType_Unmount: {
        RootUnmountRequest *req = (RootUnmountRequest*)m;

        int err_code = 0;

        UserSharedMemoryData shmem_data;
        if(ApplySharedMemoryKey(req->path_key, &shmem_data) != 0) {
            err_code = -EINVAL;
            break;
        }

        err_code = RootDir_RemoveMount(shmem_data.VirtualAddress, req->m.SourcePID);
        send_err_response(0, err_code, req->m.MsgID, req->m.SourcePID);
        Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
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
            RootDir_RemoveProcess(n->pid);
        }
        break;
        }
    }

}

void
message_handler(Message *m){
    
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

int main() {
    RootDir_Initialize();

    CREATE_NEW_MESSAGE_PTR(msg);
    msg->MsgType = CardinalMsgType_Notification;
    msg->MsgID = 0;
    PostIPCMessages(2 /*userboot PID*/, &msg, 1);

    uint32_t op_key = 0;
    uint64_t op_error = 0;
    RegisterNamespace("root", &op_key);

    FileServerHandlers handlers;

    return Server_Start(&handlers, 
                        message_handler);
}