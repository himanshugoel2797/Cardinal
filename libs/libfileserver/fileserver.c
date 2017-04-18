/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/cardinal_types.h>
#include <cardinal/shared_memory.h>
#include <cardinal/file_server.h>
#include <cardinal/syscall.h>
#include <cardinal/ipc.h>
#include "fileserver.h"

static FileSystemOpType op_mask = 0, base_op_mask = 0;
static FileServerHandlers fs_handlers;
static void (*unkn_msg_handler)(Message *);
static int exit = 0;
static int exit_code = 0;

static void
Server_HandleGetOpMask(Message *m) {
    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemGetOpMaskResponse, resp);
    resp->m.MsgID = m->MsgID;
    resp->m.MsgType = CardinalMsgType_Response;
    resp->OpMask = op_mask;

    Message *m2 = (Message*)&resp;
    PostIPCMessage(m->SourcePID, m2);
}

static void
Server_HandleOpRequest(Message *m) {

    FileSystemOpRequestHeader *h = (FileSystemOpRequestHeader*)m;

    int retVal = -EINVAL;
    uint64_t fd = 0;

    //Mask out the unsupported functions
    if(h->OpType & op_mask) {

        switch(h->OpType) {
        case FileSystemOpType_Open: {
            FileSystemOpOpen *op = (FileSystemOpOpen*)m;

            if(op->path_key.key_index == 0) {
                retVal = -EINVAL;
                break;
            }

            UserSharedMemoryData shmem_data;
            if(ApplySharedMemoryKey(&op->path_key, &shmem_data) != 0) {
                retVal = -EINVAL;
                break;
            }

            if(shmem_data.Length < op->path_offset) {
                retVal = -EINVAL;
                Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
                break;
            }

            retVal = fs_handlers.open((const char *)shmem_data.VirtualAddress + op->path_offset, op->flags, op->mode, op->access_pass, m->SourcePID, &fd);

            Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
        }
        break;
        case FileSystemOpType_Close: {
            FileSystemOpClose *op = (FileSystemOpClose*)m;

            if(op->fd == 0) {
                retVal = -EINVAL;
                break;
            }

            fs_handlers.close(op->fd, m->SourcePID);
            return;
        }
        break;
        case FileSystemOpType_Read: {
            FileSystemOpRead *op = (FileSystemOpRead*)m;

            //Map the destination key and ensure it has the desired permissions
            if(op->key.key_index == 0) {
                retVal = -EINVAL;
                break;
            }

            UserSharedMemoryData shmem_data;
            if(ApplySharedMemoryKey(&op->key, &shmem_data) != 0) {
                retVal = -EINVAL;
                break;
            }

            if(shmem_data.Length < op->len) {
                retVal = -EINVAL;
                Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
                break;
            }

            if(!(shmem_data.Flags & MemoryAllocationFlags_Write)) {
                retVal = -EINVAL;
                Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
                break;
            }

            retVal = fs_handlers.read(op->fd, op->offset, (void*)shmem_data.VirtualAddress, op->len, m->SourcePID);

            Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
        }
        break;
        case FileSystemOpType_Write: {
            FileSystemOpWrite *op = (FileSystemOpWrite*)m;

            //Map the destination key and ensure it has the desired permissions
            if(op->key.key_index == 0) {
                retVal = -EINVAL;
                break;
            }

            UserSharedMemoryData shmem_data;
            if(ApplySharedMemoryKey(&op->key, &shmem_data) != 0) {
                retVal = -EINVAL;
                break;
            }

            if(shmem_data.Length < op->len) {
                retVal = -EINVAL;
                Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
                break;
            }

            retVal = fs_handlers.write(op->fd, op->offset, (void*)shmem_data.VirtualAddress, op->len, m->SourcePID);

            Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
        }
        break;
        case FileSystemOpType_GetFileProperties: {
            FileSystemOpGetProperties *op = (FileSystemOpGetProperties*)m;

            if(op->path_key.key_index == 0) {
                retVal = -EINVAL;
                break;
            }

            UserSharedMemoryData shmem_data;
            if(ApplySharedMemoryKey(&op->path_key, &shmem_data) != 0) {
                retVal = -EINVAL;
                break;
            }

            if(shmem_data.Length < op->result_offset + sizeof(FileSystemDirectoryEntry)) {
                retVal = -EINVAL;
                Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
                break;
            }

            retVal = fs_handlers.get_file_properties((const char *)shmem_data.VirtualAddress, shmem_data.VirtualAddress + op->result_offset, m->SourcePID);

            Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);

        }
        break;
        case FileSystemOpType_Remove: {
            FileSystemOpRemove *op = (FileSystemOpRemove*)m;

            if(op->fd == 0) {
                retVal = -EINVAL;
                break;
            }

            fs_handlers.remove(op->fd, m->SourcePID);
            return;
        }
        break;
        case FileSystemOpType_Rename: {
            FileSystemOpRename *op = (FileSystemOpRename*)m;

            if(op->name_key.key_index == 0) {
                retVal = -EINVAL;
                break;
            }

            UserSharedMemoryData shmem_data;
            if(ApplySharedMemoryKey(&op->name_key, &shmem_data) != 0) {
                retVal = -EINVAL;
                break;
            }

            if(shmem_data.Length < op->name_offset) {
                retVal = -EINVAL;
                Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
                break;
            }

            retVal = fs_handlers.rename(op->fd, (const char *)shmem_data.VirtualAddress + op->name_offset, m->SourcePID);

            Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
        }
        break;
        case FileSystemOpType_Sync: {
            FileSystemOpSync *op = (FileSystemOpSync*)m;

            if(op->fd == 0) {
                retVal = -EINVAL;
                break;
            }


            fs_handlers.sync(op->fd, m->SourcePID);
            return;
        }
        break;
        default:
            if(unkn_msg_handler != NULL)
                unkn_msg_handler(m);	//TODO return error message if this handler is absent
            else
                retVal = -EINVAL;
            break;
        }
    }

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, op_response);
    op_response->m.MsgID = m->MsgID;
    op_response->m.MsgType = CardinalMsgType_Response;
    op_response->error_code = retVal;
    op_response->fd = fd;

    PostIPCMessage(m->SourcePID, (Message*)op_response);
}

static void
Server_HandleGetFileSystemInfoRequest(Message *m) {

}

int
Server_Initialize(FileServerParams p,
                  const FileServerHandlers *handlers,
                  UID (*FDAllocator)(const char *, UID, uint64_t),
                  void (*UnknownMessageHandler)(Message *)) {

    //Build an op mask from the handlers
    fs_handlers = *handlers;
    
    if(p & FileServerParams_HandlersPerFile) {
        //Replace all handlers except for open with internal ones

    }

    unkn_msg_handler = UnknownMessageHandler;

        if(handlers->open != NULL)
            op_mask |= FileSystemOpType_Open;

        if(handlers->read != NULL)
            op_mask |= FileSystemOpType_Read;

        if(handlers->write != NULL)
            op_mask |= FileSystemOpType_Write;

        if(handlers->close != NULL)
            op_mask |= FileSystemOpType_Close;

        if(handlers->remove != NULL)
            op_mask |= FileSystemOpType_Remove;

        if(handlers->rename != NULL)
            op_mask |= FileSystemOpType_Rename;

        if(handlers->sync != NULL)
            op_mask |= FileSystemOpType_Sync;

        if(handlers->get_file_properties != NULL)
            op_mask |= FileSystemOpType_GetFileProperties;

            base_op_mask = op_mask;

        return 0;
}

int
Server_Start(void) {
    //Nothing to do
    if(op_mask == 0)
        return -1;

    //loop for requests
    exit = 0;
    while(!exit) {

        CREATE_NEW_MESSAGE_PTR(msg);
        POLL_MESSAGE(msg);

        if(msg->MsgType == CardinalMsgType_Request) {

            FileSystemOpHeader *op_hdr = (FileSystemOpHeader*)msg;

            switch(op_hdr->MsgType) {
            case FileSystemRequestType_GetOpMask:
                Server_HandleGetOpMask(msg);
                break;
            case FileSystemRequestType_Op:
                Server_HandleOpRequest(msg);
                break;
            case FileSystemRequestType_GetFileSystemInfo:
                Server_HandleGetFileSystemInfoRequest(msg);
                break;
            default:
                if(UnknownMessageHandler != NULL)
                    UnknownMessageHandler(msg);	//TODO return error message if this handler is absent
                break;
            }

        } else {
            if(UnknownMessageHandler != NULL)
                UnknownMessageHandler(msg);	//TODO return error message if this handler is absent
        }

    }

    return exit_code;
}

int
Server_SetOpMask(FileSystemOpType mask) {
    op_mask = (mask & ~base_op_mask) ? op_mask : mask;
    return (mask & ~base_op_mask);
}

FileSystemOpType
Server_GetOpMask(void) {
    return op_mask;
}

void
Server_Exit(int exitcode) {
    exit_code = exitcode;
    exit = 1;
}