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
HandleMountRequest(Message *m, int (*MountHandler)(const char*, UID));

void
HandleDirentryRequest(Message *m, int (*direntry)(uint64_t fd, struct DirEntryData *data, size_t data_size));

void
HandleLinkRequest(Message *m, int (*link)(char *from, char *to));

void
HandleUnlinkRequest(Message *m, int (*unlink)(char *file));

void
HandleStatRequest(Message *m, int (*stat)(uint64_t fd, struct StatData *data));

#endif