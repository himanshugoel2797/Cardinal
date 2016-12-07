#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/file_server.h>

typedef struct {
	uint64_t Size;
} FileInfo;

int io_open(const char*, int, uint64_t, uint64_t*, UID, uint64_t*);
int io_read(uint64_t, uint64_t, void*, size_t, UID);
int io_write(uint64_t, uint64_t, void*, size_t, UID);
void io_close(uint64_t, UID);
void io_remove(uint64_t, UID);
void io_getinfo(const char*, UID, FileInfo*);
int io_rename(uint64_t, const char*, UID);
void io_sync(uint64_t, UID);

#endif