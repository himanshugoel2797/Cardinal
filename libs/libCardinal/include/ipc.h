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
#define MESSAGE_SIZE (128)

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

//! Error notification message.
#define CARDINAL_MSG_TYPE_ERROR 0

//! IPC Message.
typedef struct Message {
    UID SourcePID;          //!< The sender PID
    uint32_t MsgID;         //!< The message ID, uniquely identifies a conversation.
    uint32_t MsgType;       //!< The message type (CARDINAL_MSG_TYPE_XXXXX).
} Message;

//! An error message.
struct ErrorMessage {
    Message m;              //!< Standard required message header.
    uint64_t code;          //!< The error code.
};

//! Response buffer response footer.
typedef struct Response {
	volatile uint8_t ResponseReady;		//!< Initialized to zero. Gets set to 1 when the response is ready to read.
} Response;

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
GetIPCMessageFrom(Message *m, UID sourcePID, uint32_t msg_id) {
    return (int)Syscall3(Syscall_GetIPCMessageFrom, (uint64_t)m, (uint64_t)sourcePID, msg_id);
}

/**
 * @brief      Gets the ipc message.
 *
 * @param      m     A preallocated message buffer of size MAX_MESSAGE_SIZE
 *
 * @return     0 if no message to return, 1 if a message has been returned.
 */
static __inline int
GetIPCMessage(Message *m) {
    return GetIPCMessageFrom(m, 0, 0);
}

/**
 * @brief      Posts ipc messages.
 *
 * @param[in]  dstPID  The destination pid
 * @param      m       A list of messages to post
 * @param[in]  cnt     The number of messages in the list
 *
 * @return     -1 on failure to post any messages, the number of messages posted
 *             on success.
 */
static __inline int
PostIPCMessages(UID dstPID, Message **m, uint64_t cnt) {
    return (int)Syscall3(Syscall_PostIPCMessage, dstPID, (uint64_t)m, cnt);
}

/**
 * @brief      Request the process's IPC response buffer.
 *
 * @param      address  The address of the response buffer
 * @param      size     The size of the response buffer in bytes
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline int
RequestResponseBuffer(uint64_t *address, uint64_t *size) {
	if(address != NULL)
		*address = Syscall1(Syscall_RequestResponseBuffer, 0);

	if(size != NULL)
		*size = Syscall1(Syscall_RequestResponeBuffer, 1);

	return GetErrno();
}

static __inline int
GetResponseKey(uint32_t offset, uint32_t len, uint64_t *key) {
	if(key == NULL)
		return -EINVAL;

	*key = Syscall2(Syscall_GetResponseKey, offset, len);
	return GetErrno();
}

static __inline int
SubmitResponse(uint64_t key, void *buffer, uint32_t length) {
	if(buffer == NULL)
		return -EINVAL;

	Syscall3(Syscall_SubmitResponse, key, (uint64_t)buffer, length);
	return GetErrno();
}

static __inline int
QueryResponseKeyLength(uint64_t key, uint32_t *length) {
	if(length == NULL)
		return -EINVAL;

	*length = Syscall1(Syscall_QueryResponseKeyLength, key);
	return GetErrno();
}

#endif

/**}@*/

#endif