#ifndef _CARDINAL_LIBCSERVER_H_
#define _CARDINAL_LIBCSERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <cardinal/cardinal_types.h>
#include <cardinal/cardinal_io.h>

#include "mount_db.h"

typedef struct cserver_handlers {
    uint64_t (*open)(const char *file, int flags, int mode);
    int (*close)(uint64_t fd);
    int (*read)(uint64_t fd, void *buf, int64_t off, uint64_t whence, size_t cnt);
    int (*write)(uint64_t fd, void *buf, int64_t off, uint64_t whence, size_t cnt);
    int (*link)(char *from, char *to);
    int (*unlink)(char *file);
    int (*stat)(uint64_t fd, struct StatData *data);
    int (*direntry)(uint64_t fd, struct DirentryData *data, size_t data_size);
    int (*rename)(char *from, char *to);
    int (*mkdir)(char *name);
    int (*chmod)();
    int (*truncate)();
    int (*statfs)();
    int (*flush)();
    int (*utimens)();
    int (*lock)();
    int (*unlock)();
};

//TODO: chmod, truncate, rename, mkdir, statfs, flush, access and write time change, lock, unlock

int
cserver_main(uint64_t destCode,
             int (*MountHandler)(const char*, UID),
             int (*Mounted)(const char *parent),
             struct cserver_handlers *handlers);

#endif