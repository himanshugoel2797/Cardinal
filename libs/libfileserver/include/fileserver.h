#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include <cardinal/file_server.h>

typedef struct {
    int (*open)(const char *, int, uint64_t, uint8_t*, UID, uint64_t*);
    int (*read)(uint64_t, uint64_t, void*, uint64_t, UID);
    int (*write)(uint64_t, uint64_t, void*, uint64_t, UID);
    void (*close)(uint64_t, UID);
    void (*remove)(uint64_t, UID);
    int (*rename)(uint64_t, const char *, UID);
    void (*sync)(uint64_t, UID);
} FileServerHandlers;

int
Server_Start(FileServerHandlers *handlers,
             void (*UnknownMessageHandler)(Message *));

int
Server_SetOpMask(FileSystemOpType mask);

FileSystemOpType
Server_GetOpMask(void);

void
Server_Exit(int exitcode);

#endif