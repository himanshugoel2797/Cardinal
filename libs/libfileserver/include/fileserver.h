#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include <cardinal/cardinal_types.h>

typedef struct {
	int (*open)(const char *, int, mode_t, UID, uint64_t*);
	int (*read)(uint64_t, uint64_t, void *, size_t, UID);
	int (*write)(uint64_t, void *, size_t, UID);
	int (*close)(uint64_t, UID);
	int (*remove)(const char *, UID);
	
	int (*mkdir)(const char *, mode_t, UID);

	int (*rmdir)(const char *, UID);
	int (*rename)(const char *, const char *, UID);
	int (*addtag)(uint64_t, const char *, const char *, UID);
	int (*removetag)(uint64_t, const char *, UID);
	int (*readtags)(uint64_t, void *, size_t, UID);
	int (*sync)(uint64_t, UID);
} FileServerHandlers;

#endif