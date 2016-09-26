#ifndef _CARDINAL_LIBCSERVER_H_
#define _CARDINAL_LIBCSERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <cardinal/cardinal_types.h>
#include <cardinal/cardinal_io.h>

#include "mount_db.h"

typedef struct cserver_handlers{
	uint64_t (*open)(FileSystemObject *handlers, const char *file, int flags, int mode);
	int (*close)(FileSystemObject *handlers, uint64_t fd);
	int (*read)(FileSystemObject *handlers, uint64_t fd, void *buf, size_t cnt);
	int (*write)(FileSystemObject *handlers, uint64_t fd, void *buf, size_t cnt);
	int (*link)(FileSystemObject *handlers, char *from, char *to);
	int (*unlink)(FileSystemObject *handlers, char *file);
	int (*stat)(FileSystemObject *handlers, uint64_t fd, struct StatData *data);
	int (*direntry)(FileSystemObject *handlers, uint64_t fd, struct DirentryData *data, size_t data_size);
	int (*seek)(FileSystemObject *handlers, uint64_t fd, int64_t offset, int whence);
};

int 
cserver_main(uint64_t destCode, 
			 int (*MountHandler)(const char*, UID), 
			 int (*Mounted)(const char *parent),
			 struct cserver_handlers *handlers);

#endif