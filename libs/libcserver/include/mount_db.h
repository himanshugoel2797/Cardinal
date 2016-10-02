#ifndef _CARDINAL_MOUNT_DB_H_
#define _CARDINAL_MOUNT_DB_H_

#include <stdint.h>
#include <stdbool.h>
#include <cardinal/limits.h>

#include "list.h"

#define FD_DEFAULT_LIMIT 16384

typedef enum {
	FileSystemError_None,
	FileSystemError_AllocationFailed,
	FileSystemError_PathDoesNotExist,
	FileSystemError_PathInvalid,
	FileSystemError_NameTooLong,
	FileSystemError_PathTooLong
} FileSystemError;

typedef enum {
	FileSystemObjectType_MountPoint,
	FileSystemObjectType_Directory,
	FileSystemObjectType_File
} FileSystemObjectType;

typedef struct FileHandlers FileHandlers;

typedef struct FileSystemObject FileSystemObject;
struct FileSystemObject{
	FileSystemObject *Parent;
	FileSystemObjectType ObjectType;
	char Name[NAME_MAX];
	void *additionalData;

	union{
		uint64_t TargetPID;
		List *Children;
		FileHandlers *handlers;
	};
};

struct FileHandlers {
	uint64_t (*open)(FileSystemObject *handlers, const char *file, int flags, int mode);
	int (*close)(FileSystemObject *handlers, uint64_t fd);
	int (*read)(FileSystemObject *handlers, uint64_t fd, void *buf, size_t cnt);
	int (*write)(FileSystemObject *handlers, uint64_t fd, void *buf, size_t cnt);
};

uint64_t
HashPath(const char *str);

uint64_t
AllocateFileDescriptor(int flags, int mode, uint64_t hash, FileSystemObject *m);

bool
GetFileDescriptor(uint64_t fd, int *flags, int *mode, uint64_t *hash, FileSystemObject **a);

void
FreeFileDescriptor(uint64_t fd);

FileSystemError
CreateDirectory(char *path);

FileSystemError
CreateFile(char *path, FileHandlers *handlers);

FileSystemError
RegisterMount(char *path, uint64_t pid);

FileSystemObject*
ParsePath(char *path);

#ifdef _MOUNT_DB_PRIV_H_
void
InitializeDB(void);

void
SetMountLocation(char *loc);
#endif

void
GetMountLocation(char *loc, size_t sz);

void
SetFDLimit(uint32_t limit);

uint32_t
GetFDLimit(void);

#endif