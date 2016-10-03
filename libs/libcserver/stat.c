#include <stdint.h>

#include <time.h>
#include <cardinal/file_server.h>
#include <string.h>

#include "file_request_handlers.h"
#include "mount_db.h"

void
HandleStatRequest(Message *m, int (*stat)(uint64_t fd, struct StatData *data)) {
	struct StatRequest *stat_req = (struct StatRequest*)m;
	struct StatResponse stat_resp;

	uint64_t fd = stat_req->fd;
	int flags = 0;
	int mode = 0;
	uint64_t hash = 0;
	FileSystemObject *fs_obj = NULL;

	FILL_RESPONSE(&stat_resp, stat_req)
    stat_resp.m.Size = sizeof(struct StatResponse);
    stat_resp.msg_type = CARDINAL_MSG_TYPE_STATRESPONSE;

    if(!GetFileDescriptor(fd, &flags, &mode, &hash, &fs_obj)) {

        stat_resp.code = -1;

        if(stat != NULL)
        	stat_resp.code = stat(fd, &stat_resp.data);

        Message *m_resp = (Message*)&stat_resp;
        PostIPCMessages(&m_resp, 1);
        return;
    }

    //Get the file info and put it into the response
 	memcpy(&stat_resp.data, &fs_obj->statData, sizeof(struct StatData));

    Message *m_resp = (Message*)&stat_resp;
    PostIPCMessages(&m_resp, 1);
}