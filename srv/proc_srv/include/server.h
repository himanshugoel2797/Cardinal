#ifndef _PROC_SRV_H_
#define _PROC_SRV_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PROCESS_NAME_LEN 512

typedef enum {
	ProcessServerMessageType_R0_ProcessExistenceNotification,
	ProcessServerMessageType_CreateProcessRequest,
} ProcessServerMessageType;

typedef enum {
	ProcessServerNotificationType_ProcessCreated,
	ProcessServerNotificationType_ProcessExited,
} ProcessServerNotificationType;

typedef struct {
    Message m;
    ProcessServerMessageType MsgType;
    uint64_t exec_key;
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
	ProcessServerNotificationType MsgType;
	UID pid;
} ProcessServerNotificationType_Notification;

#ifdef __cplusplus
}
#endif

#endif