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
#define CARDINAL_MSG_TYPE_READRESPONSE 5

#define CARDINAL_MSG_TYPE_WRITEREQUEST 6
#define CARDINAL_MSG_TYPE_WRITERESPONSE 7

#define CARDINAL_MSG_TYPE_MOUNTREQUEST 8
#define CARDINAL_MSG_TYPE_MOUNTRESPONSE 9

#define CARDINAL_MSG_TYPE_DIRENTRYREQUEST 10
#define CARDINAL_MSG_TYPE_DIRENTRYRESPONSE 11

#define CARDINAL_MSG_TYPE_LINKREQUEST 12
#define CARDINAL_MSG_TYPE_LINKRESPONSE 13

#define CARDINAL_MSG_TYPE_UNLINKREQUEST 14
#define CARDINAL_MSG_TYPE_UNLINKRESPONSE 15

#define CARDINAL_MSG_TYPE_STATREQUEST 16
#define CARDINAL_MSG_TYPE_STATRESPONSE 17

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
    uint64_t buf[1];
};

struct WriteResponse {
	Message m;
	uint64_t msg_type;
	uint64_t write_size;
};

struct ReadRequest {
	Message m;
	uint64_t msg_type;
	uint64_t fd;
	uint64_t read_size;
};

struct ReadResponse {
	Message m;
	uint64_t msg_type;
	uint64_t code;
	char data[1];
};

struct MountRequest {
	Message m;
	uint64_t msg_type;
	char path[1];
};

struct MountResponse {
	Message m;
	uint64_t msg_type;
	uint64_t code;
};

struct DirEntryRequest {
	Message m;
	uint64_t fd;
	uint64_t msg_type;
	uint32_t num;
};

struct DirEntryResponse {
	Message m;
	uint64_t msg_type;
	uint64_t code;
	char data[1];
};

struct LinkRequest {
	Message m;
	uint64_t msg_type;
	char *from;
	char *to;
	char data[1];
};

struct LinkResponse {
	Message m;
	uint64_t msg_type;
	uint64_t code;
};

struct UnlinkRequest {
	Message m;
	uint64_t msg_type;
	char data[1];
};

struct UnlinkResponse {
	Message m;
	uint64_t msg_type;
	uint64_t code;
};

struct StatRequest {
	Message m;
	uint64_t msg_type;
	uint64_t fd;
};

struct StatResponse {
	Message m;
	uint64_t msg_type;
	uint64_t code;
	char data[1];
};
	
#define MAX_PATH_LEN ((4096 - sizeof(struct OpenRequest)))
#define MAX_BUF_LEN ((UINT16_MAX - sizeof(struct WriteRequest)))

#ifndef _KERNEL_

static __inline uint64_t
GetProperty(CardinalProperties prop, uint64_t type) {
	return Syscall2(Syscall_GetProperty, prop, type);
}

static __inline bool
SetProperty(CardinalProperties prop, uint64_t type, uint64_t val) {
	return Syscall3(Syscall_SetProperty, prop, type, val);
}

static __inline bool
RegisterSpecialDestination(uint64_t dst) {
	return SetProperty(CardinalProperty_SpecialDestination, dst, 0);	
}

#endif

#endif