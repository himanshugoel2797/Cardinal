#ifndef _LIB_CARDINAL_IPC_H_
#define _LIB_CARDINAL_IPC_H_

#include <stddef.h>
#include <stdint.h>
#include "syscall.h"
#include "cardinal_types.h"
#include "syscall_list.h"

#define MAX_MESSAGE_SIZE (UINT16_MAX)

#define CARDINAL_IPCDEST_NUM 512
#define CARDINAL_IPCDEST_SPECIAL (1ull << 32)
#define CARDINAL_IPCDEST_FILESERVER ((CARDINAL_IPCDEST_SPECIAL) | 1)
#define CARDINAL_IPCDEST_DISPLAYSERVER ((CARDINAL_IPCDEST_SPECIAL) | 2)
#define CARDINAL_IPCDEST_MEMORYSERVER ((CARDINAL_IPCDEST_SPECIAL) | 3)

#define CARDINAL_MSGTYPE_NONE 0
#define CARDINAL_MSGTYPE_FWD 1

typedef struct Message {
    UID SourcePID;
    UID DestinationPID;
    uint64_t MsgID;
    uint64_t MsgType;
    uint64_t MsgTypePID;	//If forwarded, this contains the initial source of the message, file servers implementing secure operations through this mechanism must ensure that the SourcePID field has the necessary permissions in order to be confident that this field is valid
    uint64_t rsv[4];	//Reserved for future use
    unsigned short Size;
} Message;

#ifndef _KERNEL_


static __inline int
GetIPCMessageFrom(Message *m, UID sourcePID, uint64_t msg_id) {
    return (int)Syscall3(Syscall_GetIPCMessageFrom, (uint64_t)m, (uint64_t)sourcePID, msg_id);
}

static __inline int
GetIPCMessage(Message *m) {
    return GetIPCMessageFrom(m, 0, 0);
}

static __inline int
PostIPCMessages(Message **m, uint64_t cnt) {
    return (int)Syscall2(Syscall_PostIPCMessage, (uint64_t)m, cnt);
}

#endif

#endif