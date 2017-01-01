/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "proc/server.h"
#include "ipc.h"
#include "io/server.h"

#include "shared_memory.h"


#include <string.h>

int
R0NotifyProcessExistence(UID pid,
                         char *name,
                         int name_len) {

    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServer_R0NotifyProcessExistence, data);
    data->m.MsgID = RequestMessageID();
    data->m.MsgType = CardinalMsgType_Notification;
    data->MsgType = ProcessServerMessageType_R0_ProcessExistenceNotification;
    data->pid = pid;
    data->process_name_key = 0;

    if(name_len != 0 && name != NULL) {
        uint64_t vAddress = 0;
        if(AllocateSharedMemory(name_len,
                                CachingModeWriteBack,
                                MemoryAllocationType_MMap,
                                MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                                &vAddress) != 0)
            return -1;

        char *dst = (char*)vAddress;
        for(int i = 0; i < name_len; i++)
            dst[i] = name[i];

        uint64_t key = 0;
        if(GetSharedMemoryKey(vAddress,
                              name_len,
                              CachingModeWriteBack,
                              MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                              &key) != 0) {
            Unmap(vAddress, name_len);
            return -1;
        }

        data->process_name_key = key;
    }

    Message *msg = (Message*)data;

    PostIPCMessages(PROCESS_SRV_PID, &msg, 1);

    if(data->process_name_key != 0) {
        uint64_t cnt = 0;
        while(1) {
            GetSharedMemoryKeyUsageCount(data->process_name_key, &cnt);
            if(cnt)
                break;
        }
        FreeSharedMemoryKey(data->process_name_key);
    }
    return 0;
}

int
RequestCreateProcess(void *exec,
                     uint64_t exec_len,
                     char **argv,
                     uint32_t argc,
                     uint64_t group_id,
                     UID *pid) {
    if(exec == NULL)
        return -1;

    if(exec_len == 0)
        return -1;

    if(argv == NULL && argc != 0)
        return -1;

    if(argc == 0 && argv != NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServer_CreateRequest, create_req);
    create_req->m.MsgID = RequestMessageID();
    create_req->m.MsgType = CardinalMsgType_Request;
    create_req->MsgType = ProcessServerMessageType_CreateProcessRequest;
    create_req->args_key = 0;
    create_req->argc = argc;
    create_req->exec_size = exec_len;
    create_req->group_id = group_id;

    if(exec_len % PAGE_SIZE)
        exec_len += PAGE_SIZE - exec_len % PAGE_SIZE;

    uint64_t vAddress = 0;
    if(AllocateSharedMemory(exec_len,
                            CachingModeWriteBack,
                            MemoryAllocationType_MMap,
                            MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                            &vAddress) != 0)
        return -1;

    memcpy((void*)vAddress, exec, create_req->exec_size);

    uint64_t key = 0;
    if(GetSharedMemoryKey(vAddress,
                          exec_len,
                          CachingModeWriteBack,
                          MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                          &key) != 0) {
        Unmap(vAddress, exec_len);
        return -1;
    }

    create_req->exec_key = key;

    //Allocate memory for the argv
    uint64_t arg_len = 0;
    for(uint32_t i = 0; i < argc; i++)
        arg_len += strlen(argv[i]);

    if(arg_len % PAGE_SIZE)
        arg_len += PAGE_SIZE - arg_len % PAGE_SIZE;

    if(arg_len > 0) {
        if(AllocateSharedMemory(arg_len,
                                CachingModeWriteBack,
                                MemoryAllocationType_MMap,
                                MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                                &vAddress) != 0)
            return -1;

        char *arg = (char*)vAddress;
        for(uint32_t i = 0; i < argc; i++) {
            strcpy(arg, argv[i]);
            arg += strlen(argv[i]) + 1;
        }

        if(GetSharedMemoryKey(vAddress,
                              arg_len,
                              CachingModeWriteBack,
                              MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                              &key) != 0) {
            Unmap(vAddress, arg_len);
            return -1;
        }

        create_req->args_key = key;
    }

    Message *msg = (Message*)create_req;

    PostIPCMessages(PROCESS_SRV_PID, &msg, 1);

    CREATE_NEW_MESSAGE_PTR(m2);
    while(!GetIPCMessageFrom(m2, PROCESS_SRV_PID, create_req->m.MsgID));

    ProcessServer_CreateRequest_Response *resp = (ProcessServer_CreateRequest_Response*)m2;
    if(pid != NULL)
        *pid = resp->pid;

    if(create_req->args_key != 0) {
        FreeSharedMemoryKey(create_req->args_key);
    }

    if(create_req->exec_key != 0) {
        FreeSharedMemoryKey(create_req->exec_key);
    }


    return (int)resp->err_code;
}


int
StartProcess(const char *str,
             char *argv[],
             int argc,
             UID dst_pid,
             uint8_t *key,
             uint64_t group_id,
             UID *pid) {

    uint64_t len = 0;
    uint64_t addr = 0;
    uint64_t read_key = 0, write_key = 0;

    uint64_t fd = 0;

    if(IO_Open(str, FileSystemOpFlag_Read, 0, key, dst_pid, &fd) != 0)
        return -1;

    FileSystemDirectoryEntry file_dat;
    if(IO_GetFileProperties(str, &file_dat, dst_pid) != 0)
        return -2;

    len = file_dat.Length;

    if(IO_AllocateBuffer(&len, &addr, &read_key, &write_key) != 0)
        return -3;

    if(IO_Read(fd, 0, read_key, file_dat.Length, dst_pid) != file_dat.Length)
        return -4;

    IO_Close(fd, dst_pid);

    int retVal = RequestCreateProcess((void*)addr, file_dat.Length, argv, argc, group_id, pid);

    IO_FreeBuffer(addr, len, read_key, write_key);

    return retVal;
}

int
SubscribeToProcessExitNotification(void) {

    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServer_SubscribeToExitNotification, create_req);
    create_req->m.MsgID = RequestMessageID();
    create_req->m.MsgType = CardinalMsgType_Request;
    create_req->MsgType = ProcessServerMessageType_SubscribeToExitNotification;

    PostIPCMessages(PROCESS_SRV_PID, (Message**)&create_req, 1);
    return 0;
}

int
SubscribeToProcessCreateNotification(void) {

    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServer_SubscribeToExitNotification, create_req);
    create_req->m.MsgID = RequestMessageID();
    create_req->m.MsgType = CardinalMsgType_Request;
    create_req->MsgType = ProcessServerMessageType_SubscribeToCreateNotification;

    PostIPCMessages(PROCESS_SRV_PID, (Message**)&create_req, 1);
    return 0;
}

void
ExitProcess(uint64_t exit_code) {

    CREATE_NEW_MESSAGE_PTR_TYPE(ProcessServer_ExitRequest, exit_req);
    exit_req->m.MsgID = RequestMessageID();
    exit_req->m.MsgType = CardinalMsgType_Request;
    exit_req->MsgType = ProcessServerMessageType_ExitProcess;

    PostIPCMessages(PROCESS_SRV_PID, (Message**)&exit_req, 1);
    while(1);
}