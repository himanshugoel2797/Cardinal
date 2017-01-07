/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LIB_CARDINAL_IPC_H_
#define _LIB_CARDINAL_IPC_H_

#include <stddef.h>
#include <stdint.h>
#include "syscall.h"
#include "thread.h"
#include "cardinal_types.h"
#include "syscall_list.h"
#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup ipc_api IPC API
 * @{
 */

//! The maximum size of a message.
#define MESSAGE_SIZE (128)

//! The process ID of the memory server.
#define MEMORY_SRV_PID 4

//! The process ID of the namespace server.
#define NAMESPACE_SRV_PID 5

//! The process ID of the process server.
#define PROCESS_SRV_PID 3

typedef enum {
    CardinalMsgType_Request,
    CardinalMsgType_Error,
    CardinalMsgType_IORequest,
    CardinalMsgType_IOResponse,
    CardinalMsgType_AuthenticationRequest,
    CardinalMsgType_Signal,
    CardinalMsgType_Response,
    CardinalMsgType_Notification,
    CardinalMsgType_Interrupt,
} CardinalMsgType;

//! IPC Message.
typedef struct Message {
    UID SourcePID;                      //!< The sender PID
    uint32_t MsgID;                     //!< The message ID, uniquely identifies a conversation.
    CardinalMsgType MsgType;            //!< The message type (CARDINAL_MSG_TYPE_XXXXX).
} Message;

typedef struct InterruptMessage {
    Message m;
    uint32_t vector;
} InterruptMessage;

//! An error message.
struct ErrorMessage {
    Message m;     //!< Standard required message header.
    int64_t code;  //!< The error code.
};

//! Response buffer response header.
typedef struct Response {
    volatile uint8_t ResponseReady;     //!< Initialized to zero. Gets set to 1 when the response is ready to read.
} Response;


struct CardinalFullMessage {
    char data[MESSAGE_SIZE];
};

#define CREATE_NEW_MESSAGE_PTR_TYPE(TYPE, XXX) struct CardinalFullMessage XXX##_0; for(int i = 0; i < MESSAGE_SIZE; i++)XXX##_0.data[i] = 0; TYPE *XXX = (TYPE*)&XXX##_0
#define CREATE_NEW_MESSAGE_PTR(XXX)  CREATE_NEW_MESSAGE_PTR_TYPE(Message, XXX)

#ifndef _KERNEL_
#define POLL_MESSAGE(XXX) WaitForMessage(MessageWaitType_Any, 0); GetIPCMessage(XXX)
#define POLL_MESSAGE_FROM_PID_MSGID(XXX, PID, MSGID) WaitForMessage(MessageWaitType_SourcePID, PID); GetIPCMessageFrom(XXX, PID, MSGID)
#define POLL_MESSAGE_FROM_PID(XXX, PID) POLL_MESSAGE_FROM_PID(XXX, PID, 0)
#define POLL_MESSAGE_MSGTYPE(XXX, MSGTYPE) WaitForMessage(MessageWaitType_MsgType, MSGTYPE); GetIPCMessageOfType(XXX, MSGTYPE)

//#define POLL_MESSAGE(XXX) while(!GetIPCMessage(XXX))
//#define POLL_MESSAGE_FROM_PID_MSGID(XXX, PID, MSGID) while(!GetIPCMessageFrom(XXX, PID, MSGID))
//#define POLL_MESSAGE_FROM_PID(XXX, PID) POLL_MESSAGE_FROM_PID(XXX, PID, 0)
//#define POLL_MESSAGE_MSGTYPE(XXX, MSGTYPE) while(!GetIPCMessageOfType(XXX, MSGTYPE))


/**
 * @brief      Gets the ipc message from the source.
 *
 * @param      m          A preallocated message buffer of size MESSAGE_SIZE
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
 * @brief      Gets the ipc message of the specified type.
 *
 * @param      m        A preallocated message bufer of size MESSAGE_SIZE
 * @param[in]  msgType  The message type
 *
 * @return     The ipc message of type.
 */
static __inline int
GetIPCMessageOfType(Message *m, CardinalMsgType msgType) {
    return (int)Syscall2(Syscall_GetIPCMessageMsgType, (uint64_t)m, msgType);
}

/**
 * @brief      Gets the ipc message.
 *
 * @param      m     A preallocated message buffer of size MESSAGE_SIZE
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
 * @brief      Allocate a block of interrupts.
 *
 * @param[in]  cnt   The count
 * @param      irq   The irq number that the block starts from
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
R01AllocateInterrupts(int cnt, int *irq) {
    if(irq == NULL)
        return -EINVAL;

    *irq = Syscall1(Syscall_R01_AllocateInterrupts, cnt);
    return GetErrno();
}

/**
 * @brief      Register to a previously allocated block of interrupts.
 *
 * @param[in]  irq   The irq
 * @param[in]  cnt   The count
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
R01RegisterInterrupts(int irq, int cnt) {
    Syscall2(Syscall_R01_RegisterForInterrupts, irq, cnt);
    return GetErrno();
}


uint32_t
RequestMessageID(void);

#endif

/**}@*/

#ifdef __cplusplus
}
#endif

#endif