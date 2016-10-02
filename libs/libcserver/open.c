#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleOpenRequest(Message *m, uint64_t (*open)(FileSystemObject *handlers, const char *file, int flags, int mode)) {
	struct OpenRequest *open_req = (struct OpenRequest*)m;
	struct OpenResponse response;

	FileSystemObject *fs_obj = ParsePath(open_req->path);
	if(fs_obj == NULL) *(uint8_t*)0 = 1;

	uint64_t fd = -1;
	
	FILL_RESPONSE(&response, open_req)
	response.m.Size = sizeof(struct OpenResponse);

	bool skip_open_handler = FALSE;

	if(fs_obj != NULL){
		if(fs_obj->ObjectType == FileSystemObjectType_File) {
			fd = fs_obj->handlers->open(fs_obj, open_req->path, (int)open_req->flags, (int)open_req->mode);	
		}
		else if(fs_obj->ObjectType == FileSystemObjectType_MountPoint) {
			//Direct request to mount point service and return the PID returned by it
			uint64_t src = open_req->m.SourcePID;

			open_req->m.SourcePID = CARDINAL_IPCDEST_FILESERVER;
			open_req->m.DestinationPID = fs_obj->TargetPID;

			PostIPCMessages((Message**)&open_req, 1);

			struct timespec t;
			t.tv_sec = 0;
			t.tv_nsec = 100;

			while(GetIPCMessageFrom((Message*)&response, fs_obj->TargetPID, open_req->m.MsgID) != 1)
				nanosleep(&t, NULL);

			response.m.SourcePID = CARDINAL_IPCDEST_FILESERVER;
			response.m.DestinationPID = src;

			Message *ma = (Message*)&response;
			PostIPCMessages(&ma, 1);
			return;
		}
		else if(fs_obj->ObjectType == FileSystemObjectType_Directory) {
			//If it can be determined that the goal was a directory, open the directory, else defer to the open handler
		}
		else if(open != NULL) {
			//Call the open handler provided by the application to handle this situation
		}
	} else if(open != NULL) {

	}

	response.msg_type = CARDINAL_MSG_TYPE_OPENRESPONSE;
	response.fd = fd;
	response.targetPID = CARDINAL_IPCDEST_FILESERVER;

	if(fd == -1)__asm__("hlt");
	Message *ma = (Message*)&response;
	PostIPCMessages(&ma, 1);
	return;
}