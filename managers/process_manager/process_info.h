#ifndef _CARDINAL_PROCESS_INFO_H_
#define _CARDINAL_PROCESS_INFO_H_

#include "types.h"
#include "list.h"
#include "memory.h"
#include "synchronization.h"

#include "libs/libCardinal/include/ipc.h"

//! The maximum length of a process name.
#define MAX_PROCESS_NAME_LEN (256)

//! The maximum number of pending messages.
#define MAX_PENDING_MESSAGE_CNT 256

//! The size of the Process Local Storage (PLS).
#define MAX_PLS_SIZE KiB(64)

//! The PID of the root of the process tree.
#define ROOT_PID 1

/**
 * The process status.
 */
typedef enum {
    ProcessStatus_Stopped,      //!< The process is stopped.
    ProcessStatus_Sleeping,     //!< The process is sleeping.
    ProcessStatus_Terminating,  //!< The process is terminating.
    ProcessStatus_Executing     //!< The process is executing.
} ProcessStatus;

/**
 * The process priority.
 */
typedef enum {
    ProcessPriority_Normal,     //!< Normal priority.
    ProcessPriority_High        //!< High priority.
} ProcessPriority;

typedef enum {
    ProcessErrors_None = 0,
    ProcessErrors_Unknown = (1 << 0),
    ProcessErrors_UIDNotFound = (1 << 1),
} ProcessErrors;

typedef struct ProcessInformation {
    UID                         ID;
    UID                         UserID;
    UID                         GroupID;    //GroupIDs are used to control permission levels

    char                        Name[MAX_PROCESS_NAME_LEN];
    ProcessStatus               Status;
    ManagedPageTable            *PageTable;
    uint64_t                    HeapBreak;
    uint32_t                    ExitStatus;
    List                        *Children;
    List                        *ThreadIDs;
    List                        *PendingMessages;

    void                        *PLS;
    size_t                      PLSSize;

    bool                        HandlingMessage;
    Spinlock                    MessageLock;

    struct ProcessInformation   *Parent;

    uint32_t                    reference_count;
    Spinlock                    lock;
} ProcessInformation;


#endif