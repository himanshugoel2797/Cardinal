#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include <cardinal/cardinal_types.h>

typedef struct {
    int (*open)(const char *, int, uint64_t, uint8_t*, UID, uint64_t*);
    int (*read)(uint64_t, uint64_t, void*, uint64_t, UID);
    int (*write)(uint64_t, uint64_t, void*, uint64_t, UID);
    void (*close)(uint64_t, UID);
    void (*remove)(uint64_t, UID);
    int (*get_info)(const char *, UID);
    int (*rename)(uint64_t, const char *, UID);
    void (*sync)(uint64_t, UID);
} FileServerHandlers;

#endif