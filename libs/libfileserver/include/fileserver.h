#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include <cardinal/cardinal_types.h>

typedef struct {
	int (*open)(const char *, int, mode_t, UID);
	int (*read)(int, uint64_t, void *, size_t, UID);
	int (*write)(int, void *, size_t, UID);
	int (*close)(int, UID);
	int (*remove)(const char *, UID);
	
	int (*mkdir)(const char *, mode_t, UID);

	int (*rmdir)(const char *, UID);
	int (*rename)(const char *, const char *, UID);

	int (*addtag)(int, const char *, const char *, UID);
	int (*removetag)(int, const char *, UID);
	int (*readtags)(int, void *, size_t, UID);
} FileServerHandlers;

#endif