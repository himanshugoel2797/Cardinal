/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "cardinal_types.h"
#include "shared_memory.h"
#include "file_server.h"
#include "syscall.h"
#include "ipc.h"
#include <string.h>
#include "io/server.h"
#include "helpers.h"

static uint32_t
fill_struct(FileSystemOpRequestHeader *op,
            FileSystemOpType op_type) {

    op->h.m.MsgID = RequestMessageID();
    op->h.m.MsgType = CardinalMsgType_Request;
    op->h.MsgType = FileSystemRequestType_Op;
    op->OpType = op_type;

    return op->h.m.MsgID;
}

int
IO_AllocateBuffer(uint64_t* len,
                  uint64_t* vAddress,
                  Key_t* read_key,
                  Key_t* write_key) {

    if(*len % PAGE_SIZE)
        *len += PAGE_SIZE - *len % PAGE_SIZE;

    if(AllocateSharedMemory(*len,
                            CachingModeWriteBack,
                            MemoryAllocationType_MMap,
                            MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                            vAddress) != 0)
        return -1;

    //The write_key is the key to be passed to IO_Write
    if(GetSharedMemoryKey(*vAddress,
                          *len,
                          CachingModeWriteBack,
                          MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                          write_key) != 0) {
        Unmap(*vAddress, *len);
        return -1;
    }

    //The read_key is the key to be passed to IO_Read
    if(GetSharedMemoryKey(*vAddress,
                          *len,
                          CachingModeWriteBack,
                          MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_Present,
                          read_key) != 0) {
        Unmap(*vAddress, *len);
        return -1;
    }
    return 0;
}

int
IO_FreeBuffer(uint64_t address,
              uint64_t len,
              Key_t *read_key,
              Key_t *write_key) {

    if(Unmap(address, len) != 0)
        return -1;

    if(FreeSharedMemoryKey(read_key) != 0)
        return -1;

    if(FreeSharedMemoryKey(write_key) != 0)
        return -1;

    return 0;
}

int
IO_Open(const char * path,
        int flags,
        uint64_t mode,
        uint8_t *access_key,
        UID pid,
        uint64_t *fd) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpOpen, op);
    uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_Open);

    uint64_t path_len = strlen(path);
    uint64_t shmem_len = path_len;
    char * vAddr = NULL;

    if(__card_setup_shmem(&shmem_len, 0, &op->path_key, (uint64_t*)&vAddr) != 0)
        return -1;

    strcpy(vAddr, path);

    op->path_offset = 0;
    op->flags = flags;
    op->mode = mode;

    for(int i = 0; i < KEY_BYTES; i++)
        op->access_pass[i] = access_key[i];

    if(PostIPCMessage(pid, (Message*)op) != 1)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
    POLL_MESSAGE_FROM_PID_MSGID((Message*)resp, pid, msgID);

    FreeSharedMemoryKey(&op->path_key);
    Unmap((uint64_t)vAddr, shmem_len);

    *fd = resp->fd;
    return resp->error_code;
}

int
IO_Read(uint64_t fd,
        uint64_t offset,
        Key_t key,
        uint64_t len,
        UID pid) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpRead, op);
    uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_Read);

    op->fd = fd;
    op->offset = offset;
    op->key = key;
    op->len = len;

    if(PostIPCMessage(pid, (Message*)op) != 1)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
    POLL_MESSAGE_FROM_PID_MSGID((Message*)resp, pid, msgID);

    return resp->error_code;
}

int
IO_Write(uint64_t fd,
         uint64_t offset,
         Key_t key,
         uint64_t len,
         UID pid) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpWrite, op);
    uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_Write);

    op->fd = fd;
    op->offset = offset;
    op->key = key;
    op->len = len;

    if(PostIPCMessage(pid, (Message*)op) != 1)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
    POLL_MESSAGE_FROM_PID_MSGID((Message*)resp, pid, msgID);

    return resp->error_code;
}

int
IO_GetFileProperties(const char *path,
                     FileSystemDirectoryEntry *info,
                     UID pid) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpGetProperties, op);
    uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_GetFileProperties);

    uint64_t path_len = strlen(path);
    uint64_t shmem_len = path_len + sizeof(FileSystemDirectoryEntry) + 1;
    char * vAddr = NULL;

    if(__card_setup_shmem(&shmem_len, MemoryAllocationFlags_Write, &op->path_key, (uint64_t*)&vAddr) != 0)
        return -1;

    strcpy(vAddr, path);

    op->result_offset = path_len + 1;

    if(PostIPCMessage(pid, (Message*)op) != 1)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
    POLL_MESSAGE_FROM_PID_MSGID((Message*)resp, pid, msgID);

    memcpy(info, vAddr + op->result_offset, sizeof(FileSystemDirectoryEntry));

    FreeSharedMemoryKey(&op->path_key);
    Unmap((uint64_t)vAddr, shmem_len);

    return resp->error_code;
}

void
IO_Close(uint64_t fd,
         UID pid) {
    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpClose, op);
    fill_struct(&op->op_h, FileSystemOpType_Close);
    op->fd = fd;
    PostIPCMessage(pid, (Message*)op);
}

void
IO_Remove(uint64_t fd,
          UID pid) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpClose, op);
    fill_struct(&op->op_h, FileSystemOpType_Close);
    op->fd = fd;
    PostIPCMessage(pid, (Message*)op);
}

int
IO_Rename(uint64_t fd,
          const char *dst,
          UID pid) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpRename, op);
    uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_Rename);

    uint64_t path_len = strlen(dst);
    uint64_t shmem_len = path_len;
    char * vAddr = NULL;

    if(__card_setup_shmem(&shmem_len, 0, &op->name_key, (uint64_t*)&vAddr) != 0)
        return -1;

    strcpy(vAddr, dst);

    op->name_offset = 0;
    op->fd = fd;

    if(PostIPCMessage(pid, (Message*)op) != 1)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
    POLL_MESSAGE_FROM_PID_MSGID((Message*)resp, pid, msgID);

    FreeSharedMemoryKey(&op->name_key);
    Unmap((uint64_t)vAddr, shmem_len);

    return resp->error_code;
}

void
IO_Sync(uint64_t fd,
        UID pid) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpClose, op);
    fill_struct(&op->op_h, FileSystemOpType_Close);
    op->fd = fd;
    PostIPCMessage(pid, (Message*)op);
}