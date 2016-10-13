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
#define CARDINAL_IPCDEST_LOCAL (1ull << 33)
#define CARDINAL_IPCDEST_LOCAL_NAMESPACEREGISTRY ((CARDINAL_IPCDEST_LOCAL) | (CARDINAL_IPCDEST_SPECIAL) | 1)
#define CARDINAL_IPCDEST_GLOBAL_NAMESPACEREGISTRY ((CARDINAL_IPCDEST_SPECIAL) | 1)
#define CARDINAL_IPCDEST_GLOBAL_DISPLAYSERVER ((CARDINAL_IPCDEST_SPECIAL) | 2)
#define CARDINAL_IPCDEST_GLOBAL_MEMORYSERVER ((CARDINAL_IPCDEST_SPECIAL) | 3)

#define CARDINAL_MSG_TYPE_IO 1
#define CARDINAL_MSG_TYPE_SIGNAL 2

typedef struct Message {
    UID SourcePID;
    UID DestinationPID;
    uint64_t MsgID;
    uint64_t MsgType;
    uint64_t rsv[4];	//Reserved for future use
    unsigned short Size;
} Message;

struct ErrorMessage {
	Message m;
	uint64_t code;
};

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