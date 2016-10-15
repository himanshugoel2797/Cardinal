#ifndef _LIB_CARDINAL_IPC_H_
#define _LIB_CARDINAL_IPC_H_

#include <stddef.h>
#include <stdint.h>
#include "syscall.h"
#include "cardinal_types.h"
#include "syscall_list.h"

/**
 * \defgroup ipc_api IPC API
 * @{
 */

//! The maximum size of a message.
#define MAX_MESSAGE_SIZE (UINT16_MAX)

//! The maximum number of special destinations.
#define CARDINAL_IPCDEST_NUM 512

//! Identifies a system reserved destination.
#define CARDINAL_IPCDEST_SPECIAL (1ull << 32)

#define CARDINAL_IPCDEST_LOCAL (1ull << 33)
#define CARDINAL_IPCDEST_LOCAL_NAMESPACEREGISTRY ((CARDINAL_IPCDEST_LOCAL) | (CARDINAL_IPCDEST_SPECIAL) | 1)
#define CARDINAL_IPCDEST_GLOBAL_NAMESPACEREGISTRY ((CARDINAL_IPCDEST_SPECIAL) | 1)
#define CARDINAL_IPCDEST_GLOBAL_DISPLAYSERVER ((CARDINAL_IPCDEST_SPECIAL) | 2)
#define CARDINAL_IPCDEST_GLOBAL_MEMORYSERVER ((CARDINAL_IPCDEST_SPECIAL) | 3)

//! I/O request message.
#define CARDINAL_MSG_TYPE_IO 1

//! Signal notification message.
#define CARDINAL_MSG_TYPE_SIGNAL 2

//! IPC Message.
typedef struct Message {
    UID SourcePID;          //!< The sender PID
    UID DestinationPID;     //!< The destination PID
    uint64_t MsgID;         //!< The message ID, uniquely identifies a conversation.
    uint64_t MsgType;       //!< The message type (CARDINAL_MSG_TYPE_XXXXX).
    uint64_t rsv[4];	    //!< Reserved for future use.
    unsigned short Size;    //!< The size of the message.
} Message;

//! An error message.
struct ErrorMessage {
    Message m;              //!< Standard required message header.
    uint64_t code;          //!< The error code.
};

#ifndef _KERNEL_

/**
 * @brief      Gets the ipc message from the source.
 *
 * @param      m          A preallocated message buffer of size MAX_MESSAGE_SIZE
 * @param[in]  sourcePID  The source pid (0 for any)
 * @param[in]  msg_id     The message identifier (0 for any)
 *
 * @return     0 if no message to return, 1 if a message has been returned.
 */
static __inline int
GetIPCMessageFrom(Message *m, UID sourcePID, uint64_t msg_id) {
    return (int)Syscall3(Syscall_GetIPCMessageFrom, (uint64_t)m, (uint64_t)sourcePID, msg_id);
}

/**
 * @brief      Gets the ipc message.
 *
 * @param      m     A preallocated message buffer of size MAX_MESSAGE_SIZE
 *
 * @return     The ipc message.
 */
static __inline int
GetIPCMessage(Message *m) {
    return GetIPCMessageFrom(m, 0, 0);
}

/**
 * @brief      Posts ipc messages.
 *
 * @param      m     A list of messages to post
 * @param[in]  cnt   The number of messages in the list
 *
 * @return     If 'i' is the number of messages sent, -(i + 1) on error, i on success.
 */
static __inline int
PostIPCMessages(Message **m, uint64_t cnt) {
    return (int)Syscall2(Syscall_PostIPCMessage, (uint64_t)m, cnt);
}

#endif

/**}@*/

#endif