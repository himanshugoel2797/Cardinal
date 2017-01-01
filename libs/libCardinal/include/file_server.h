/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_FILE_SERVER_H_
#define _CARDINAL_FILE_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "sibyl/server.h"
#include "cardinal_io.h"
#include "syscall_property.h"
#include "syscall.h"
#include "ipc.h"
#include "limits.h"

typedef enum {
    FileSystemRequestType_GetOpMask = 0,
    FileSystemRequestType_Op,
    FileSystemRequestType_GetFileSystemInfo,
} FileSystemRequestType;

typedef enum {
    FileTypeFlag_DataFile = (1 << 0),
    FiletypeFlag_Directory = (1 << 1),
} FileTypeFlag;

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
    FileSystemOpType_Rename = (1 << 6),
    FileSystemOpType_Sync = (1 << 7),
    FileSystemOpType_GetFileProperties = (1 << 8),
    FileSystemOpType_SetFileProperties = (1 << 9),
} FileSystemOpType;

typedef enum {
    FileSystemOpFlag_Read = (1 << 0),
    FileSystemOpFlag_Write = (1 << 1),
    FileSystemOpFlag_Create = (1 << 2),
    FileSystemOpFlag_Directory = (1 << 3),
    FileSystemOpFlag_Exclusive = (1 << 4),  //One writer, many readers OR many readers, no writer
    FileSystemOpFlag_Append = (1 << 5),
    FileSystemOpFlag_Locked = (1 << 6),     //One reader/writer
} FileSystemOpFlag;

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
    uint64_t path_key;
    uint64_t path_offset;
    int flags;
    uint64_t mode;
    uint8_t access_pass[KEY_BYTES];
} FileSystemOpOpen;

typedef struct {
    FileSystemOpRequestHeader op_h;
    uint64_t path_key;
    uint64_t result_offset;
} FileSystemOpGetProperties;

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
    FileSystemOpRequestHeader op_h;
    uint64_t fd;
    uint64_t name_key;
    uint64_t name_offset;
} FileSystemOpRename;

typedef struct {
    Message m;
    int error_code;
    uint64_t fd;
} FileSystemOpResponse;


typedef struct {
    uint64_t EntryCount;
} FileSystemDirectoryHeader;

typedef struct {
    char Name[NAME_MAX];
    FileTypeFlag FileType;
    FileSystemOpFlag AccessMode;
    uint64_t Length;
    char ReadKID[KID_SIZE_BYTES];
    char WriteKID[KID_SIZE_BYTES];
    char ExecuteKID[KID_SIZE_BYTES];
} FileSystemDirectoryEntry;

typedef struct {
    FileSystemDirectoryHeader hdr;
    FileSystemDirectoryEntry Entries[0];
} FileSystemDirectoryData;



#endif