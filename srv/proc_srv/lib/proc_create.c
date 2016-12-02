#include "server.h"
#include <cardinal/shared_memory.h>
#include <libipc/ipc.h>


int
R0NotifyProcessExistence(UID pid,
					     char *name,
						 int name_len) {

	ProcessServer_R0NotifyProcessExistence data;
	data.m.MsgID = RequestMessageID();
	data.m.MsgType = CardinalMsgType_Notification;
	data.MsgType = ProcessServerMessageType_R0_ProcessExistenceNotification;
	data.pid = pid;
	data.process_name_key = 0;

	if(name_len != 0 && name != NULL){
	uint64_t vAddress = 0;
	if(AllocateSharedMemory(name_len, 
							CachingModeWriteBack,
							MemoryAllocationType_MMap,
							MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
							&vAddress) != 0)
			return -1;

	char *dst = (char*)vAddress;
	for(int i = 0; i < name_len; i++)
		dst[i] = name[i];

	uint64_t key = 0;
	if(GetSharedMemoryKey(vAddress,
					   name_len,
					   CachingModeWriteBack,
					   MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
					   &key) != 0)
	{
		Unmap(vAddress, name_len);
		return -1;
	}

		data.process_name_key = key;
	}

	Message *msg = (Message*)&data;

	PostIPCMessages(PROCESS_SRV_PID, &msg, 1);

	if(data.process_name_key != 0){
		uint64_t cnt = 0;
		while(1){
			GetSharedMemoryKeyUsageCount(data.process_name_key, &cnt);
			if(cnt)
				break;
		}
		FreeSharedMemoryKey(data.process_name_key);
	}
	return 0;
}