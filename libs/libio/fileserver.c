#include <cardinal/cardinal_types.h>
#include <cardinal/shared_memory.h>
#include <cardinal/file_server.h>
#include <cardinal/syscall.h>
#include <cardinal/ipc.h>
#include <libipc/ipc.h>
#include <string.h>
#include "fileserver.h"

uint32_t fill_struct(FileSystemOpRequestHeader *op,
				 FileSystemOpType op_type) {
	op->h.m.MsgID = RequestMessageID();
	op->h.m.MsgType = CardinalMsgType_IORequest;
	op->h.MsgType = FileSystemRequestType_Op;
	op->OpType = op_type;

	return op->h.m.MsgID;
}

int 
setup_shmem(uint64_t *len, 
			MemoryAllocationFlags target, 
			uint64_t *key, 
			uint64_t *vAddress) {

	*len += PAGE_SIZE - *len % PAGE_SIZE;

	if(AllocateSharedMemory(*len, 
							CachingModeWriteBack,
							MemoryAllocationType_MMap,
							MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
							vAddress) != 0)
			return -1;

	if(GetSharedMemoryKey(*vAddress,
					   *len,
					   CachingModeWriteBack,
					   target | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
					   key) != 0)
	{
		Unmap(*vAddress, *len);
		return -1;
	}

	return 0;
}


int
IO_Open(const char * path, 
		int flags, 
		uint64_t mode,
		uint64_t *access_key,
		UID pid,
		uint64_t *fd) {

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpOpen, op);
	uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_Open);

	uint64_t path_len = strlen(path);
	uint64_t shmem_len = path_len;
	char * vAddr = NULL;

	if(setup_shmem(&shmem_len, 0, &op->filename_key, (uint64_t*)&vAddr) != 0)
		return -1;

	strcpy(vAddr, path);

	op->filename_offset = 0;
	op->flags = flags;
	op->mode = mode;
	
	for(int i = 0; i < KEY_BYTES; i++)
		op->access_pass[i] = access_key[i];

	if(PostIPCMessages(pid, (Message**)&op, 1) != 1)
		return -1;

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
	while(!GetIPCMessageFrom((Message*)resp, pid, msgID));

	uint64_t cnt = 0;
	while(1){
		GetSharedMemoryKeyUsageCount(op->filename_key, &cnt);
		if(cnt)
			break;
	}
	FreeSharedMemoryKey(op->filename_key);

	*fd = resp->fd;
	return resp->error_code;
}

int 
IO_Read(uint64_t fd, 
		uint64_t offset,
		uint64_t key, 
		uint64_t len,
		UID pid) {

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpRead, op);
	uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_Read);

	op->fd = fd;
	op->offset = offset;
	op->key = key;
	op->len = len;

	if(PostIPCMessages(pid, (Message**)&op, 1) != 1)
		return -1;

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
	while(!GetIPCMessageFrom((Message*)resp, pid, msgID));

	return resp->error_code;
}

int 
IO_Write(uint64_t fd, 
		 uint64_t offset,
		 uint64_t key, 
		 uint64_t len,
		 UID pid) {

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpWrite, op);
	uint32_t msgID = fill_struct(&op->op_h, FileSystemOpType_Write);

	op->fd = fd;
	op->offset = offset;
	op->key = key;
	op->len = len;

	if(PostIPCMessages(pid, (Message**)&op, 1) != 1)
		return -1;

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpResponse, resp);
	while(!GetIPCMessageFrom((Message*)resp, pid, msgID));

	return resp->error_code;
}

void
IO_Close(uint64_t fd,
		 UID pid) {
	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpClose, op);
	fill_struct(&op->op_h, FileSystemOpType_Close);
	op->fd = fd;
	PostIPCMessages(pid, (Message**)&op, 1);
}

void
IO_Remove(uint64_t fd,
		  UID pid) {

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpClose, op);
	fill_struct(&op->op_h, FileSystemOpType_Close);
	op->fd = fd;
	PostIPCMessages(pid, (Message**)&op, 1);
}

int 
IO_Rename(uint64_t fd, 
		  const char *dst,
		  UID pid) {

}

void 
IO_GetInfo(const char *path, 
		   UID pid, 
		   FileInfo *info) {

}

void
IO_Sync(uint64_t fd,
		UID pid) {

	CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpClose, op);
	fill_struct(&op->op_h, FileSystemOpType_Close);
	op->fd = fd;
	PostIPCMessages(pid, (Message**)&op, 1);
}