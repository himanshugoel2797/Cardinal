#ifndef _CARDINAL_FILE_SERVER_H_
#define _CARDINAL_FILE_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "cardinal_io.h"
#include "syscall_property.h"
#include "syscall.h"
#include "ipc.h"

#define PATH_MAX KiB(64)
#define KEY_BYTES 64

typedef enum {
    FileSystemRequestType_GetOpMask = 0,
    FileSystemRequestType_Op,
    FileSystemRequestType_GetFileSystemInfo,
} FileSystemRequestType;

typedef enum {
    FileSystemType_Hierarchical = (1 << 0),
    FileSystemType_Relational = (1 << 1)
} FileSystemType;

typedef enum {
    FileSystemOpType_Open = (1 << 0),
    FileSystemOpType_Read = (1 << 1),
    FileSystemOpType_Write = (1 << 2),
    FileSystemOpType_Close = (1 << 3),
    FileSystemOpType_Remove = (1 << 4),
    FileSystemOpType_GetInfo = (1 << 5),
    FileSystemOpType_Rename = (1 << 6),
    FileSystemOpType_Sync = (1 << 7),
} FileSystemOpType;

typedef struct {
    Message m;
    FileSystemRequestType MsgType;
} FileSystemOpHeader;

typedef struct {
    FileSystemOpHeader h;
} FileSystemGetOpMaskRequest;

typedef struct {
    Message m;
    FileSystemOpType OpMask;
} FileSystemGetOpMaskResponse;

typedef struct {
    FileSystemOpHeader h;
    FileSystemOpType OpType;
} FileSystemOpRequestHeader;

typedef struct {
    FileSystemOpRequestHeader op_h;
    uint64_t filename_key;
    uint64_t filename_offset;
    int flags;
    uint64_t mode;
    uint8_t access_pass[KEY_BYTES];
} FileSystemOpOpen;

typedef struct {
    FileSystemOpRequestHeader op_h;
    uint64_t fd;
} FileSystemOpClose;

typedef struct {
    FileSystemOpRequestHeader op_h;
    uint64_t fd;
} FileSystemOpRemove;

typedef struct {
    FileSystemOpRequestHeader op_h;
    uint64_t fd;
} FileSystemOpSync;

typedef struct {
    FileSystemOpRequestHeader op_h;
    uint64_t fd;
    uint64_t offset;
    uint64_t key;
    uint64_t len;
} FileSystemOpRead;

typedef struct {
    FileSystemOpRequestHeader op_h;
    uint64_t fd;
    uint64_t offset;
    uint64_t key;
    uint64_t len;
} FileSystemOpWrite;

typedef struct {
    Message m;
    int error_code;
    uint64_t fd;
} FileSystemOpResponse;

#endif