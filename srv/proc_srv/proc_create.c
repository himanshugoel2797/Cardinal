#include "common.h"
#include "proc_db.h"
#include "server.h"
#include "program.h"
#include <string.h>
#include <cardinal/process.h>
#include <cardinal/shared_memory.h>
#include <libipc/ipc.h>

void
send_existence_notification(UID pid) {
    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServerNotificationType_Notification, note);
    note->m.MsgID = RequestMessageID();
    note->m.MsgType = CardinalMsgType_Notification;
    note->MsgType = ProcessServerNotificationType_ProcessCreated;
    note->pid = pid;

    //Post this message to the various servers that need this info.
    Message *m = (Message*)note;
    PostIPCMessages(MEMORY_SRV_PID, &m, 1);
    PostIPCMessages(NAMESPACE_SRV_PID, &m, 1);
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
        __asm__("hlt");
    }

    char *argv = NULL;
    uint32_t argc = 0;
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
    R0_CreateProcess(msg->m.SourcePID, 0, &pid);

    LoadAndStartApplication(pid, elf_loc, elf_size, argv, argc, arg_len);

    //Add the process to the database
    ProcDB_AddProcess(pid, argv);

    Unmap((uint64_t)exec_data.VirtualAddress, exec_data.Length);
    Unmap((uint64_t)arg_data.VirtualAddress, arg_data.Length);


    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServer_CreateRequest_Response, resp);
    resp->m.MsgID = msg->m.MsgID;
    resp->m.MsgType = CardinalMsgType_Response;
    resp->pid = pid;
    resp->err_code = 0;

    Message *m2 = (Message*)resp;
    PostIPCMessages(m->SourcePID, &m2, 1);

    send_existence_notification(pid);
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