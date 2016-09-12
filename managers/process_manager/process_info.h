#ifndef _CARDINAL_PROCESS_INFO_H_
#define _CARDINAL_PROCESS_INFO_H_

#include "types.h"
#include "list.h"
#include "memory.h"
#include "synchronization.h"

#include "libs/libCardinal/include/ipc.h"

#define MAX_PROCESS_NAME_LEN (256)
#define MAX_PENDING_MESSAGE_CNT 256
#define MAX_PLS_SIZE KiB(64)

#define ROOT_PID 1

typedef enum {
    ProcessStatus_Stopped,
    ProcessStatus_Sleeping,
    ProcessStatus_Terminating,
    ProcessStatus_Executing,
    ProcessStatus_Zombie
} ProcessStatus;

typedef enum {
    ProcessPriority_Normal,
    ProcessPriority_High
} ProcessPriority;

typedef enum {
    ProcessPermissions_None = 0,
    ProcessPermissions_IRQAccess = (1 << 0),
    ProcessPermissions_SystemPermissions = (1 << 1)
} ProcessPermissions;

typedef enum {
    ProcessSyscallFlags_None = 0,
    ProcessSyscallFlags_PermissionsLocked = (1 << 0),
    ProcessSyscallFlags_DriverPermissions = (1 << 1),
} ProcessSyscallFlags;

typedef enum {
    ProcessErrors_None = 0,
    ProcessErrors_Unknown = (1 << 0),
    ProcessErrors_UIDNotFound = (1 << 1),
    ProcessErrors_DescriptorNotFound = (1 << 2)
} ProcessErrors;

typedef struct ProcessInformation {
    UID                         ID;
    UID                         UserID;
    UID                         GroupID;    //GroupIDs are used to control filesystem namespace levels

    char                        Name[MAX_PROCESS_NAME_LEN];
    ProcessStatus               Status;
    ProcessPermissions          Permissions;
    ManagedPageTable            *PageTable;
    ProcessSyscallFlags         SyscallFlags;
    uint64_t                    HeapBreak;
    uint32_t                    ExitStatus;
    List                        *Children;
    List                        *ThreadIDs;
    List                        *PendingMessages;

    void                        *PLS;

    bool                        HandlingMessage;
    Spinlock                    MessageLock;

    char                        *WorkingDirectory;
    struct ProcessInformation   *Parent;

    uint32_t                    reference_count;
    Spinlock                    lock;
} ProcessInformation;


#endif