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
HandleRemoveRequest(Message *m, int (*remove)(char *file));

void
HandleStatRequest(Message *m, int (*stat)(uint64_t fd, struct StatData *data));

//TODO forward requests at mount point
void
HandleRenameRequest(Message *m, int (*rename)(char *from, char *to));

//TODO forward requests at mount point
void
HandleMkdirRequest(Message *m, int (*mkdir)(char *name));

//TODO forward requests at mount point
void
HandleChmodRequest(Message *m, int (*chmod)(char *name, int mode));

//TODO forward requests at mount point
void
HandleTruncateRequest(Message *m, int (*truncate)(char *name, uint64_t len));

//TODO forward requests at mount point
void
HandleStatFsRequest(Message *m, int (*statfs)(struct FS_StatData *data));

//TODO forward requests at mount point
void
HandleFlushRequest(Message *m, int (*flush)());

//TODO forward requests at mount point
void
HandleUtimeNSRequest(Message *m, int (*utimens)(char *name, struct timespec times[2], int flags));

void
HandleLockRequest(Message *m, int (*lock)(uint64_t fd, int flags));

void
HandleUnlockRequest(Message *m, int (*unlock)(uint64_t fd, int flags));

void
HandleFd2PathRequest(Message *m, int (*fd2path)(uint64_t fd, char *path, size_t *path_len));

#endif