#ifndef _LIB_CARDINAL_IPC_H_
#define _LIB_CARDINAL_IPC_H_

#include <stddef.h>
#include <stdint.h>
#include <syscall.h>
#include <cardinal_types.h>

#define MESSAGE_SIZE (4096 - 16)

typedef struct Message {
    char Content[MESSAGE_SIZE];
    UID SourcePID;
    UID DestinationPID;
} Message;

#ifndef _KERNEL_
static __inline int
GetIPCMessage(Message *m) {
	uint64_t ans = Syscall1(Syscall_GetIPCMessage, (uint64_t)m);
	return (int)ans;
}

static __inline int
GetIPCMessageFrom(Message *m, UID sourcePID) {
	uint64_t ans = Syscall2(Syscall_GetIPCMessageFrom, (uint64_t)m, (uint64_t)sourcePID);
	return (int)ans;
}

static __inline int
PostIPCMessage(Message *m) {
	uint64_t ans = Syscall1(Syscall_PostIPCMessage, (uint64_t)m);
	return (int)ans;
}
#endif

#endif