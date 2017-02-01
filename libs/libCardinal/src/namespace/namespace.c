/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <string.h>

#include "cardinal_types.h"
#include "ipc.h"
#include "namespace/server.h"


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

    int err = PostIPCMessage(NAMESPACE_SRV_PID, msg_p);

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

    int err = PostIPCMessage(NAMESPACE_SRV_PID, msg_p);

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

    int err = PostIPCMessage(NAMESPACE_SRV_PID, msg_p);

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