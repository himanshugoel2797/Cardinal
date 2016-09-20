#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include <cardinal/ipc.h>
#include <cardinal/file_server.h>

#include "mount_db.h"
#include "interface_provider.h"
#include "file_request_handlers.h"

int cserver_main(void) {
    
	RegisterSpecialDestination(CARDINAL_IPCDEST_FILESERVER);
	InitializeDB();
	InitializeInterface();


	struct timespec t;
	t.tv_sec = 0;
	t.tv_nsec = 100;

	Message *m = malloc(MAX_MESSAGE_SIZE);
	while(true) {
		while(!GetIPCMessageFrom(m, 0, 0))
			nanosleep(&t, NULL);

		struct OpenRequest * test_req = (struct OpenRequest*) m;

		switch(test_req->msg_type) {
			case CARDINAL_MSG_TYPE_OPENREQUEST:
				HandleOpenRequest(m);
			break;
			case CARDINAL_MSG_TYPE_CLOSEREQUEST:
				HandleCloseRequest(m);
			break;
			case CARDINAL_MSG_TYPE_READREQUEST:
				HandleReadRequest(m);
			break;
			case CARDINAL_MSG_TYPE_WRITEREQUEST:
				HandleWriteRequest(m);
			break;
			case CARDINAL_MSG_TYPE_MOUNTREQUEST:
				HandleMountRequest(m);
			break;
			case CARDINAL_MSG_TYPE_DIRENTRYREQUEST:
				HandleDirentryRequest(m);
			break;
			case CARDINAL_MSG_TYPE_LINKREQUEST:
				HandleLinkRequest(m);
			break;
			case CARDINAL_MSG_TYPE_UNLINKREQUEST:
				HandleUnlinkRequest(m);
			break;
			case CARDINAL_MSG_TYPE_STATREQUEST:
				HandleStatRequest(m);
			break;
            case CARDINAL_MSG_TYPE_FD2PATHREQUEST:
            
            break;
		}

	}
	
	free(m);
}