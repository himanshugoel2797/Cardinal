#include <cardinal/cardinal_types.h>
#include <cardinal/shared_memory.h>
#include <cardinal/file_server.h>
#include <cardinal/syscall.h>
#include <cardinal/ipc.h>
#include "fileserver.h"

static FileSystemOpType op_mask = 0;
static FileServerHandlers *fs_handlers;
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
    PostIPCMessages(m->SourcePID, &m2, 1);
}

static void
Server_HandleOpRequest(Message *m) {

    FileSystemOpRequestHeader *h = (FileSystemOpRequestHeader*)m;

    if(!(h->OpType & op_mask)) {
        //Respond with 'unsupported' as the response.
    }

    int retVal = -EINVAL;
    uint64_t fd = 0;
    switch(h->OpType) {
    case FileSystemOpType_Open: {
        FileSystemOpOpen *op = (FileSystemOpOpen*)m;

        if(op->filename_key == 0) {
            retVal = -EINVAL;
            break;
        }

        UserSharedMemoryData shmem_data;
        if(ApplySharedMemoryKey(op->filename_key, &shmem_data) != 0) {
            retVal = -EINVAL;
            break;
        }

        if(shmem_data.Length < op->filename_offset) {
            retVal = -EINVAL;
            Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
            break;
        }

        retVal = fs_handlers->open((const char *)shmem_data.VirtualAddress + op->filename_offset, op->flags, op->mode, op->access_pass, m->SourcePID, &fd);

        Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
    }
    break;
    case FileSystemOpType_Close: {
        FileSystemOpClose *op = (FileSystemOpClose*)m;

        if(op->fd == 0) {
            retVal = -EINVAL;
            break;
        }

        fs_handlers->close(op->fd, m->SourcePID);
        return;
    }
    break;
    case FileSystemOpType_Read: {
        FileSystemOpRead *op = (FileSystemOpRead*)m;

        //Map the destination key and ensure it has the desired permissions
        if(op->key == 0) {
            retVal = -EINVAL;
            break;
        }

        UserSharedMemoryData shmem_data;
        if(ApplySharedMemoryKey(op->key, &shmem_data) != 0) {
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

        retVal = fs_handlers->read(op->fd, op->offset, (void*)shmem_data.VirtualAddress, op->len, m->SourcePID);

        Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
    }
    break;
    case FileSystemOpType_Write: {
        FileSystemOpWrite *op = (FileSystemOpWrite*)m;

        //Map the destination key and ensure it has the desired permissions
        if(op->key == 0) {
            retVal = -EINVAL;
            break;
        }

        UserSharedMemoryData shmem_data;
        if(ApplySharedMemoryKey(op->key, &shmem_data) != 0) {
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

        retVal = fs_handlers->read(op->fd, op->offset, (void*)shmem_data.VirtualAddress, op->len, m->SourcePID);

        Unmap((uint64_t)shmem_data.VirtualAddress, shmem_data.Length);
    }
    break;
    case FileSystemOpType_Remove: {
        FileSystemOpRemove *op = (FileSystemOpRemove*)m;

        if(op->fd == 0) {
            retVal = -EINVAL;
            break;
        }

        fs_handlers->remove(op->fd, m->SourcePID);
        return;
    }
    break;
    case FileSystemOpType_GetInfo: {

    }
    break;
    case FileSystemOpType_Rename: {

    }
    break;
    case FileSystemOpType_Sync: {
        FileSystemOpSync *op = (FileSystemOpSync*)m;

        if(op->fd == 0) {
            retVal = -EINVAL;
            break;
        }

        fs_handlers->sync(op->fd, m->SourcePID);
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

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, op_response);
    op_response->m.MsgID = m->MsgID;
    op_response->m.MsgType = CardinalMsgType_IOResponse;
    op_response->error_code = retVal;
    op_response->fd = fd;

    PostIPCMessages(m->SourcePID, (Message**)&op_response, 1);
}

static void
Server_HandleGetFileSystemInfoRequest(Message *m) {

}

int
Server_Start(FileServerHandlers *handlers,
             void (*UnknownMessageHandler)(Message *)) {

    //Build an op mask from the handlers
    fs_handlers = handlers;
    unkn_msg_handler = UnknownMessageHandler;

    if(op_mask == 0) {
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

    }


    //Nothing to do
    if(op_mask == 0)
        return -1;

    //loop for requests
    while(!exit) {

        CREATE_NEW_MESSAGE_PTR(msg);
        POLL_MESSAGE(msg);

        if(msg->MsgType == CardinalMsgType_IORequest) {

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
    op_mask = mask;
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