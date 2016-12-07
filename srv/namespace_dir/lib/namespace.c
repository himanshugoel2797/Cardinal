#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include <libipc/ipc.h>
#include <string.h>

#include "server.h"


int
RegisterNamespace(char *name,
                  uint32_t *key) {
    if(key == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(NamespaceRegistrationRequest, request);
    request->m.MsgID = RequestMessageID();
    request->m.MsgType = CardinalMsgType_Request;
    request->MsgType = NamespaceServerMessageType_Registration;
    strncpy(request->namespace_name, name, NAMESPACE_NAME_LEN);

    Message *msg_p = (Message*)request;

    int err = PostIPCMessages(NAMESPACE_SRV_PID, &msg_p, 1);

    if(err != 1)
        return err;

    *key = request->m.MsgID;
    return 0;    
}

int
UnregisterNamespace(char *name,
                    uint32_t *key) {
    if(key == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(NamespaceRegistrationRequest, request);
    request->m.MsgID = RequestMessageID();
    request->m.MsgType = CardinalMsgType_Request;
    request->MsgType = NamespaceServerMessageType_Removal;
    strncpy(request->namespace_name, name, NAMESPACE_NAME_LEN);

    Message *msg_p = (Message*)request;

    int err = PostIPCMessages(NAMESPACE_SRV_PID, &msg_p, 1);

    if(err != 1)
        return err;

    *key = request->m.MsgID;
    return 0;    
}

int
RetrieveNamespace(char *name,
                  uint32_t *key) {

    if(key == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(NamespaceRegistrationRequest, request);
    request->m.MsgID = RequestMessageID();
    request->m.MsgType = CardinalMsgType_Request;
    request->MsgType = NamespaceServerMessageType_Retrieval;
    strncpy(request->namespace_name, name, NAMESPACE_NAME_LEN);

    Message *msg_p = (Message*)request;

    int err = PostIPCMessages(NAMESPACE_SRV_PID, &msg_p, 1);

    if(err != 1)
        return err;

    *key = request->m.MsgID;
    return 0;    
}

int
IsNamespaceRetrieved(uint32_t key, 
                     UID *pid,
                     uint64_t *error) {
    if(pid == NULL | error == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR(msg_buf);
    NamespaceRetrievalResponse *resp = (NamespaceRetrievalResponse*)msg_buf;

    int err = GetIPCMessageFrom((Message*)resp, NAMESPACE_SRV_PID, key);

    if(err == 1) {
        *error = resp->result;
        *pid = resp->pid;
    }

    return (err == 1);
}

int
IsNamespaceRequestReady(uint32_t key,
                        uint64_t *error) {
    if(error == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR(msg_buf);
    NamespaceRetrievalResponse *resp = (NamespaceRetrievalResponse*)msg_buf;

    int err = GetIPCMessageFrom((Message*)resp, NAMESPACE_SRV_PID, key);

    if(err == 1) {
        *error = resp->result;
    }

    return (err == 1);
}