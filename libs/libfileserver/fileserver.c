#include <cardinal/cardinal_types.h>
#include <cardinal/file_server.h>
#include <cardinal/ipc.h>

static FileSystemOpType op_mask = 0;
static FileSystemHandlers *fs_handlers;
static void (*unkn_msg_handler)(Message *);
static int exit = 0;
static int exit_code = 0;

static void
Server_HandleGetOpMask(Message *m) {

}

static void
Server_HandleOpRequest(Message *m) {

	if(unkn_msg_handler != NULL)
		unkn_msg_handler(m);	//TODO return error message if this handler is absent
}

static void
Server_HandleGetFileSystemInfoRequest(Message *m) {

}

int
Server_Start(FileSystemHandlers *handlers,
			 void (*UnknownMessageHandler)(Message *)) {

	//Build an op mask from the handlers
	fs_handlers = handlers;
	unkn_msg_handler = UnknownMessageHandler;

	if(op_mask == 0){
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

	//if(handlers-> != NULL)
	//	op_mask |= FileSystemOpType_GetInfo;

	if(handlers->mkdir != NULL)
		op_mask |= FileSystemOpType_MakeDir;

	//if(handlers-> != NULL)
	//	op_mask |= FileSystemOpType_ReadDir;

	if(handlers->rmdir != NULL)
		op_mask |= FileSystemOpType_RemoveDir;

	if(handlers->rename != NULL)
		op_mask |= FileSystemOpType_Rename;

	if(handlers->addtag != NULL)
		op_mask |= FileSystemOpType_AddTag;

	if(handlers->removetag != NULL)
		op_mask |= FileSystemOpType_RemoveTag;

	if(handlers->readtags != NULL)
		op_mask |= FileSystemOpType_ReadTags;
	}

	//Nothing to do
	if(op_mask == 0)
		return -1;

	//loop for requests
	while(!exit) {

		CREATE_NEW_MESSAGE_PTR(msg);
		POLL_MESSAGE(msg);

		if(msg->MsgType == CardinalMsgType_IO) {

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

		}else {
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