/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _PROC_SRV_H_
#define _PROC_SRV_H_

#include "../cardinal_types.h"
#include "../ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PROCESS_NAME_LEN 512

typedef enum {
    ProcessServerMessageType_R0_ProcessExistenceNotification,
    ProcessServerMessageType_CreateProcessRequest,
    ProcessServerMessageType_SubscribeToExitNotification,
    ProcessServerMessageType_SubscribeToCreateNotification,
    ProcessServerMessageType_ExitProcess
} ProcessServerMessageType;

typedef enum {
    ProcessServerNotificationType_ProcessCreated,
    ProcessServerNotificationType_ProcessExited,
} ProcessServerNotificationType;

typedef struct {
    Message m;
    ProcessServerMessageType MsgType;
    uint32_t argc;
    Key_t exec_key;
    uint64_t exec_size;
    Key_t args_key;
    uint64_t group_id;
} ProcessServer_CreateRequest;

typedef struct {
    Message m;
    UID pid;
    uint64_t err_code;
} ProcessServer_CreateRequest_Response;

typedef struct {
    Message m;
    ProcessServerMessageType MsgType;
    UID pid;
    Key_t process_name_key;
} ProcessServer_R0NotifyProcessExistence;

typedef struct {
    Message m;
    ProcessServerMessageType MsgType;
} ProcessServer_SubscribeToExitNotification;

typedef ProcessServer_SubscribeToExitNotification ProcessServer_SubscribeToCreateNotification;

typedef struct {
    Message m;
    ProcessServerNotificationType MsgType;
    uint64_t exit_code;
} ProcessServer_ExitRequest;

typedef struct {
    Message m;
    ProcessServerNotificationType MsgType;
    UID pid;
} ProcessServerNotificationType_Notification;


int
R0NotifyProcessExistence(UID pid,
                         char *name,
                         int name_len);

int
RequestCreateProcess(void *exec,
                     uint64_t exec_len,
                     char **argv,
                     uint32_t argc,
                     uint64_t group_id,
                     UID *pid);


int
StartProcess(const char *str,
             char *argv[],
             int argc,
             UID dst_pid,
             uint8_t *key,
             uint64_t group_id,
             UID *pid);

int
SubscribeToProcessExitNotification(void);

int
SubscribeToProcessCreateNotification(void);

void
ExitProcess(uint64_t exit_code);

#ifdef __cplusplus
}
#endif

#endif