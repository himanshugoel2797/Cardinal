#ifndef _CARDINAL_DIRECTORY_SERVER_FILE_REQUEST_HANDLERS_H_
#define _CARDINAL_DIRECTORY_SERVER_FILE_REQUEST_HANDLERS_H_

#include <cardinal/ipc.h>

//TODO: Implement system to handle list of service provider mounts and server files

#define FILL_RESPONSE(a, b) (a)->m.MsgID = (b)->m.MsgID; (a)->m.SourcePID = (b)->m.DestinationPID; (a)->m.DestinationPID = (b)->m.SourcePID;

void
HandleOpenRequest(Message *m, uint64_t (*open_c)(const char *file, int flags, int mode));

void
HandleCloseRequest(Message *m, int (*close)(uint64_t fd));

void
HandleReadRequest(Message *m, int (*read)(uint64_t fd, void *buf, int64_t offset, uint64_t whence, size_t cnt));

void
HandleWriteRequest(Message *m, int (*write)(uint64_t fd, void *buf, int64_t offset, uint64_t whence, size_t cnt));

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