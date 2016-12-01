#ifndef _CARDINAL_FILE_SERVER_H_
#define _CARDINAL_FILE_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "cardinal_io.h"
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

#define CARDINAL_MSG_TYPE_REMOVEREQUEST 14
#define CARDINAL_MSG_TYPE_REMOVERESPONSE 15

#define CARDINAL_MSG_TYPE_STATREQUEST 16
#define CARDINAL_MSG_TYPE_STATRESPONSE 17

#define CARDINAL_MSG_TYPE_FD2PATHREQUEST 18
#define CARDINAL_MSG_TYPE_FD2PATHRESPONSE 19

#define CARDINAL_MSG_TYPE_MOUNTEVENT 20

#define CARDINAL_MSG_TYPE_RENAMEREQUEST 21
#define CARDINAL_MSG_TYPE_RENAMERESPONSE 22

#define CARDINAL_MSG_TYPE_MKDIRREQUEST 23
#define CARDINAL_MSG_TYPE_MKDIRRESPONSE 24

#define CARDINAL_MSG_TYPE_CHMODREQUEST 25
#define CARDINAL_MSG_TYPE_CHMODRESPONSE 26

#define CARDINAL_MSG_TYPE_TRUNCATEREQUEST 27
#define CARDINAL_MSG_TYPE_TRUNCATERESPONSE 28

#define CARDINAL_MSG_TYPE_STATFSREQUEST 29
#define CARDINAL_MSG_TYPE_STATFSRESPONSE 30

#define CARDINAL_MSG_TYPE_FLUSHREQUEST 31

#define CARDINAL_MSG_TYPE_UTIMENSREQUEST 32
#define CARDINAL_MSG_TYPE_UTIMENSRESPONSE 33

#define CARDINAL_MSG_TYPE_LOCKREQUEST 34
#define CARDINAL_MSG_TYPE_LOCKRESPONSE 35

#define CARDINAL_MSG_TYPE_UNLOCKREQUEST 36
#define CARDINAL_MSG_TYPE_UNLOCKRESPONSE 37

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
    int64_t offset;
    uint64_t whence;
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
    int64_t offset;
    uint64_t whence;
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

struct RemoveRequest {
    Message m;
    uint64_t msg_type;
    char data[1];
};

struct RemoveResponse {
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
    struct StatData data;
};

struct Fd2PathRequest {
    Message m;
    uint64_t msg_type;
    uint64_t fd;
};

struct Fd2PathResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
    char path[1];
};

struct MountEvent {
    Message m;
    uint64_t msg_type;
    char path[1];
};

struct RenameRequest {
    Message m;
    uint64_t msg_type;
    uint16_t from_offset;
    uint16_t to_offset;
    char data[1];
};

struct RenameResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
};

struct MkdirRequest {
    Message m;
    uint64_t msg_type;
    char path[1];
};

struct MkdirResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
};

struct ChmodRequest {
    Message m;
    uint64_t msg_type;
    int mode;
    char path[1];
};

struct ChmodResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
};

struct TruncateRequest {
    Message m;
    uint64_t msg_type;
    uint64_t len;
    char path[1];
};

struct TruncateResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
};

struct StatfsRequest {
    Message m;
    uint64_t msg_type;
};

struct StatfsResponse {
    Message m;
    uint64_t msg_type;
    struct FS_StatData data;
};

struct FlushRequest {
    Message m;
    uint64_t msg_type;
};

struct UtimensRequest {
    Message m;
    uint64_t msg_type;
    uint64_t flags;
    char path[1];
};

struct UtimensResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
    uint64_t ns_0;
    uint64_t ns_1;
};

struct LockRequest {
    Message m;
    uint64_t msg_type;
    uint64_t flags;
    char path[1];
};

struct LockResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
};

struct UnlockRequest {
    Message m;
    uint64_t msg_type;
    uint64_t flags;
    char path[1];
};

struct UnlockResponse {
    Message m;
    uint64_t msg_type;
    uint64_t code;
};

#define MAX_PATH_LEN ((4096))
#define MAX_BUF_LEN ((MAX_MESSAGE_SIZE - sizeof(struct WriteRequest)))

#endif