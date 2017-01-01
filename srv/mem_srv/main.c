/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/mem/server.h>
#include <cardinal/proc/server.h>
#include <cardinal/ipc.h>

#include "common.h"
#include "mem_db.h"


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

    CREATE_NEW_MESSAGE_PTR(msg);
    msg->MsgType = CardinalMsgType_Notification;
    msg->MsgID = 0;
    PostIPCMessages(2 /*userboot PID*/, &msg, 1);

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
            }


        } else
            HandleSystemMessages(msg);
    }
}