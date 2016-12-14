#ifndef _CARDINAL_PROCESS_INFO_H_
#define _CARDINAL_PROCESS_INFO_H_

#include "types.h"
#include "list.h"
#include "memory.h"
#include "synchronization.h"

/**
 * \addtogroup proc_man
 */

#include "libs/libCardinal/include/ipc.h"

//! The maximum number of pending messages.
#define MAX_PENDING_MESSAGE_CNT 4096

//! The PID of the root of the process tree.
#define ROOT_PID 1

//! The maximum number of keys a process may have for itself.
#define MAX_KEYS_PER_PROCESS 4000

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

/**
 * Process Errors.
 */
typedef enum {
    ProcessErrors_None = 0,                     //!< No Error.
    ProcessErrors_Unknown = (1 << 0),           //!< Unknown Error.
    ProcessErrors_UIDNotFound = (1 << 1),       //!< Invalid UID.
    ProcessErrors_InvalidParameters = (1 << 2), //!< Invalid parameter.
    ProcessErrors_OutOfMemory = (1 << 3),       //!< Out of memory.
} ProcessErrors;

/**
 * Process information/state.
 */
typedef struct ProcessInformation {
    UID                         ID;                         //!< The Process ID.
    UID                         UserID;                     //!< The User ID of the owner.
    UID                         GroupID;                    //!< GroupIDs are used to control permission levels

    ProcessStatus               Status;                     //!< The status of the process.
    uint32_t                    ExitStatus;                 //!< The process exit status.
    uint64_t                    HeapBreak;                  //!< The heap break.
    ManagedPageTable            *PageTable;                 //!< The page table.
    List                        *Children;                  //!< The process's children processes.
    List                        *ThreadInfos;                 //!< The threads that belong to this process.
    List                        *PendingMessages;           //!< The pending messages.
    uint64_t                    *Keys;                      //!< The keys provided to or owned by this process.
    uint32_t                    LowestFreeKeyIndex;

    bool                        HandlingMessage;            //!< Is the process currently handling a message?
    Spinlock                    MessageLock;                //!< Message access synchronization.

    struct ProcessInformation   *Parent;                    //!< Pointer to the parent of the process.
    uint32_t                    InterruptsUsed;             //!< Tracks if the process has requested interrupt notifications.
    uint32_t                    reference_count;            //!< Reference count
    Spinlock                    lock;                       //!< Read/Write synchronization.
} ProcessInformation;

typedef struct ResponseBufferKeyData {
    uint32_t offset;
    uint32_t length;
} ResponseBufferKeyData;

/**@}*/

#endif