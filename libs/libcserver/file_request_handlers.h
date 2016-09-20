#ifndef _CARDINAL_DIRECTORY_SERVER_FILE_REQUEST_HANDLERS_H_
#define _CARDINAL_DIRECTORY_SERVER_FILE_REQUEST_HANDLERS_H_

#include <cardinal/ipc.h>

//TODO: Implement system to handle list of service provider mounts and server files

#define FILL_RESPONSE(a, b) (a)->m.MsgID = (b)->m.MsgID; (a)->m.SourcePID = (b)->m.DestinationPID; (a)->m.DestinationPID = (b)->m.SourcePID;

void
HandleOpenRequest(Message *m);

void
HandleCloseRequest(Message *m);

void
HandleReadRequest(Message *m);

void
HandleWriteRequest(Message *m);

void
HandleMountRequest(Message *m);

void
HandleDirentryRequest(Message *m);

void
HandleLinkRequest(Message *m);

void
HandleUnlinkRequest(Message *m);

void
HandleStatRequest(Message *m);

#endif