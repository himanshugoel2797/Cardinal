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
    uint64_t exec_key;
    uint64_t exec_size;
    uint64_t args_key;
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
    uint64_t process_name_key;
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