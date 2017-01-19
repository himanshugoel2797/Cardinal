/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/proc/server.h>
#include <cardinal/process.h>
#include <cardinal/shared_memory.h>
#include <cardinal/ipc.h>
#include <string.h>
#include <list/list.h>

#include "common.h"
#include "proc_db.h"
#include "program.h"


static List *exit_subs = NULL;
static List *create_subs = NULL;

void
proc_initialize(void) {
    exit_subs = List_Create();
    create_subs = List_Create();

    List_AddEntry(exit_subs, (void*)NAMESPACE_SRV_PID);
    List_AddEntry(exit_subs, (void*)MEMORY_SRV_PID);

    List_AddEntry(create_subs, (void*)NAMESPACE_SRV_PID);
    List_AddEntry(create_subs, (void*)MEMORY_SRV_PID);
}

void
send_exit_notification(UID pid) {
    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServerNotificationType_Notification, note);
    note->m.MsgID = RequestMessageID();
    note->m.MsgType = CardinalMsgType_Notification;
    note->MsgType = ProcessServerNotificationType_ProcessExited;
    note->pid = pid;

    //Post this message to the various servers that need this info.
    for(uint64_t i = 0; i < List_Length(exit_subs); i++) {
        PostIPCMessages((UID)List_EntryAt(exit_subs, i), (Message**)&note, 1);
    }
}

void
send_existence_notification(UID pid) {
    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServerNotificationType_Notification, note);
    note->m.MsgID = RequestMessageID();
    note->m.MsgType = CardinalMsgType_Notification;
    note->MsgType = ProcessServerNotificationType_ProcessCreated;
    note->pid = pid;

    //Post this message to the various servers that need this info.
    for(uint64_t i = 0; i < List_Length(create_subs); i++) {
        PostIPCMessages((UID)List_EntryAt(create_subs, i), (Message**)&note, 1);
    }
}


void
subscribe_creation(Message *m) {

    if(ProcDB_GetCreateSubsFlag(m->SourcePID) != 1) {
        ProcDB_SetCreateSubsFlag(m->SourcePID, 1);
        List_AddEntry(create_subs, (void*)m->SourcePID);
    }
}

void
subscribe_exit(Message *m) {

    if(ProcDB_GetExitSubsFlag(m->SourcePID) != 1) {
        ProcDB_SetExitSubsFlag(m->SourcePID, 1);
        List_AddEntry(exit_subs, (void*)m->SourcePID);
    }
}

void
exit_process(Message *m) {

    if(ProcDB_GetCreateSubsFlag(m->SourcePID)) {
        for(uint64_t i = 0; i < List_Length(create_subs); i++) {
            if((UID)List_EntryAt(create_subs, i) == m->SourcePID) {
                List_Remove(create_subs, i);
                break;
            }
        }
    }

    if(ProcDB_GetExitSubsFlag(m->SourcePID)) {
        for(uint64_t i = 0; i < List_Length(create_subs); i++) {
            if((UID)List_EntryAt(create_subs, i) == m->SourcePID) {
                List_Remove(create_subs, i);
                break;
            }
        }
    }

    ProcessServer_ExitRequest *exit_req = (ProcessServer_ExitRequest*)m;
    R0_KillProcess(m->SourcePID, exit_req->exit_code);

    send_exit_notification(m->SourcePID);
}

void
create_process_handler(Message *m) {
    ProcessServer_CreateRequest *msg = (ProcessServer_CreateRequest*)m;

    if(msg->exec_key == 0) {
        //TODO Respond with error.
        __asm__("hlt");
    }

    if(msg->argc != 0 && msg->args_key == 0) {
        //TODO respond with error.
        __asm__("hlt" :: "a"(m->SourcePID));
    }

    char *argv = NULL;
    uint32_t argc = msg->argc;
    uint64_t maxLen = 0;

    UserSharedMemoryData arg_data;
    if(msg->args_key != 0) {
        if(ApplySharedMemoryKey(msg->args_key, &arg_data) != 0) {
            //TODO send error response
            __asm__("hlt");
        }
        argv = arg_data.VirtualAddress;
        maxLen = arg_data.Length;
    }

    uint64_t arg_len = 0;

    for(uint32_t i = 0; i < argc; i++) {
        uint32_t s_arg_len = strlen(argv);
        argv += s_arg_len + 1;
        arg_len += s_arg_len;

        if(arg_len > maxLen) {
            Unmap((uint64_t)arg_data.VirtualAddress, arg_data.Length);

            //TODO respond with error.
            __asm__("hlt");
            break;
        }
    }

    if(argv != NULL)
        argv = arg_data.VirtualAddress;

    UserSharedMemoryData exec_data;
    if(ApplySharedMemoryKey(msg->exec_key, &exec_data) != 0) {
        //TODO send error response
        __asm__("hlt");
    }

    void *elf_loc = exec_data.VirtualAddress;
    uint64_t elf_size = msg->exec_size;
    if(elf_size > exec_data.Length) {
        //TODO send error response
        __asm__("hlt");
    }

    UID pid = 0;
    R0_CreateProcess(msg->m.SourcePID, msg->group_id, &pid);

    LoadAndStartApplication(pid, elf_loc, elf_size, argv, argc, arg_len);

    //Add the process to the database
    ProcDB_AddProcess(pid, argv);
    send_existence_notification(pid);

    R0_StartProcess(pid);

    Unmap((uint64_t)exec_data.VirtualAddress, exec_data.Length);
    Unmap((uint64_t)arg_data.VirtualAddress, arg_data.Length);


    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServer_CreateRequest_Response, resp);
    resp->m.MsgID = msg->m.MsgID;
    resp->m.MsgType = CardinalMsgType_Response;
    resp->pid = pid;
    resp->err_code = 0;

    Message *m2 = (Message*)resp;
    PostIPCMessages(m->SourcePID, &m2, 1);

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
        Unmap((uint64_t)data.VirtualAddress,
              data.Length);
    }

    send_existence_notification(msg->pid);
}