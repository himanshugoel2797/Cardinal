#ifndef _CARDINAL_PERMISSION_MAN_H_
#define _CARDINAL_PERMISSION_MAN_H_

#include <stdint.h>
#include <stddef.h>
#include <cardinal/ipc.h>

typedef enum {
    PermissionSharingMode_SingleUse = 1
                                      PermissionSharingMode_Permanent,
} PermissionSharingMode;

typedef enum {
    PermissionStatus_Available = (1 << 0),
    PermissionStatus_Shareable = (1 << 1)
} PermissionStatus;

typedef struct {
    Message m;
    UID target_process;
    uint64_t key;
} PermissionStatusRequest;

typedef struct {
    Message m;
    PermissionStatus status;
    PermissionSharingMode sharing_mode;
} PermissionStatusResponse;

typedef struct {
    Message m;
    PermissionStatus status;
    PermissionSharingMode sharing_mode;
} CreatePermission;

typedef struct {
    Message m;
    uint64_t key;
} CreatePermissionResponse;

typedef struct {
    Message m;
    UID target_process;
    PermissionSharingMode sharing_mode;
    uint64_t key;
} SharePermission;

#endif