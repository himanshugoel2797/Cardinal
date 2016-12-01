#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include <libipc/ipc.h>
#include <string.h>

#include "server.h"

MemoryAllocationErrors
CreateGrant(uint64_t address, 
			uint32_t length) {

}

int
RequestCreateGrant(uint64_t address,
				   uint32_t length,
				   uint32_t *key) {
	if(key == NULL | length == 0 | address == 0)
		return -1;

	GrantCreationRequest create_req;
	create_req.m.MsgID = RequestMessageID();
	create_req.m.MsgType = CardinalMsgType_Request;
	create_req.MsgType = MemoryServerMessageType_GrantCreationRequest;
	create_req.Length = length;
	create_req.Address = address;

	Message *msg_p = (Message*)&create_req;

	int err = PostIPCMessages(MEMORY_SRV_PID, &msg_p, 1);

	if(err != 1)
		return err;

	*key = create_req.m.MsgID;
	return 0;
}

int
IsGrantCreated(uint32_t key,
			   MemoryAllocationErrors *error,
			   uint64_t *read_key,
			   uint64_t *write_key) {
	
	if(read_key == NULL | write_key == NULL | error == NULL)
		return -1;

	CREATE_NEW_MESSAGE_PTR(msg_buf);
	GrantCreationResponse *resp = (GrantCreationResponse*)msg_buf;

	int err = GetIPCMessageFrom((Message*)resp, MEMORY_SRV_PID, key);

	if(err == 1){
		*error = resp->error;
		*write_key = resp->WriteKey;
		*read_key = resp->ReadKey;
	}

	return (err == 1);
}


MemoryAllocationErrors
ApplyGrant(uint64_t key) {

}

int
RequestApplyGrant(uint64_t grant_key,
				uint32_t *key) {

}

int
IsGrantApplied(uint32_t key,
			   MemoryAllocationErrors *error,
			   uint64_t *address,
			   MemoryAllocationFlags *access_perms) {

}


MemoryAllocationErrors
UnapplyGrant(uint64_t key) {

}

int
RequestUnapplyGrant(uint64_t grant_key,
				  uint32_t *key) {

}

int
IsGrantUnapplied(uint32_t key,
			     MemoryAllocationErrors *error) {

}

MemoryAllocationErrors
DestroyGrant(uint64_t key) {

}

int
RequestDestroyGrant(uint64_t grant_key,
				  uint32_t *key) {

}

int
IsGrantDestroyed(uint32_t key,
			     MemoryAllocationErrors *error) {

}