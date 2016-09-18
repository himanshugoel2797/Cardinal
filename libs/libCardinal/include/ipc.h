#ifndef _LIB_CARDINAL_IPC_H_
#define _LIB_CARDINAL_IPC_H_

#include <stddef.h>
#include <stdint.h>
#include "syscall.h"
#include "cardinal_types.h"
#include "syscall_list.h"

#define MAX_MESSAGE_SIZE (UINT16_MAX)

#define CARDINAL_IPCDEST_NUM 32
#define CARDINAL_IPCDEST_FILESERVER ((1ull << 32) | 1) 
#define CARDINAL_IPCDEST_DISPLAYSERVER ((1ull << 32) | 2)

typedef struct Message {
    UID SourcePID;
    UID DestinationPID;
    uint64_t MsgID;
    unsigned short Size;
} Message;

#ifndef _KERNEL_

int
GetIPCMessage(Message *m);
int
GetIPCMessageFrom(Message *m, UID sourcePID, uint64_t msg_id);

int
PostIPCMessages(Message **m, uint64_t cnt);

#endif

#endif