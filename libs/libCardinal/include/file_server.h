#ifndef _CARDINAL_FILE_SERVER_H_
#define _CARDINAL_FILE_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "syscall_property.h"
#include "syscall.h"
#include "ipc.h"


#define CARDINAL_MSG_TYPE_OPENREQUEST 1
#define CARDINAL_MSG_TYPE_OPENRESPONSE 2
#define CARDINAL_MSG_TYPE_CLOSEREQUEST 3
#define CARDINAL_MSG_TYPE_READREQUEST 4
#define CARDINAL_MSG_TYPE_WRITEREQUEST 5

struct OpenRequest {
	Message m;
	uint64_t msg_type;
    uint64_t flags;
    uint64_t mode;
    char path[1];
};

struct OpenResponse {
	Message m;
	uint64_t msg_type;
	uint64_t fd;
	uint64_t targetPID;
};

struct CloseRequest {
	Message m;
	uint64_t msg_type;
    uint64_t fd;
};

struct WriteRequest {
	Message m;
	uint64_t msg_type;
    uint64_t fd;
    bool lock;
    uint64_t buf[1];
};

struct ReadRequest {
	Message m;
	uint64_t msg_type;
	uint64_t fd;
	
};

#define MAX_PATH_LEN ((4096 - sizeof(struct OpenRequest)))
#define MAX_BUF_LEN ((UINT16_MAX - sizeof(struct WriteRequest)))

#ifndef _KERNEL_

static __inline bool
RegisterSpecialDestination(uint64_t dst) {
	return Syscall3(Syscall_SetProperty, CardinalProperty_SpecialDestination, dst, 0);	
}

#endif

#endif