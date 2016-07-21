#ifndef _CARDINAL_FILE_FD_H_
#define _CARDINAL_FILE_FD_H_

#include "types.h"
#include "synchronization.h"

#define MAX_FILENAME_LENGTH 256

typedef enum {
	FileTreeEntryType_File,
	FileTreeEntryType_Directory,
	FileTreeEntryType_FIFO,
	FileTreeEntryType_Pipe,
	FileTreeEntryType_Block,
	FileTreeEntryType_Symlink
} FileTreeEntryType;

typedef enum {
	FileTreeEntryFlags_None,
	FileTreeEntryFlags_CloseOnExec
} FileTreeEntryFlags;

typedef enum {
	FileDescriptorAccessMode_Read,
	FileDescriptorAccessMode_Write
} FileDescriptorAccessMode;

typedef struct FileTreeEntry {
	wchar_t 				Name[MAX_FILENAME_LENGTH];
	uint64_t 				AccessPermissions;
	FileTreeEntryFlags 		Flags;
	FileTreeEntryType 		Type;
	UID 					FileSystemDriverPID;
	UID						Identifier;

	Spinlock				ReadLock;
	Spinlock				WriteLock;

	uint32_t 				reference_count;

	struct FileTreeEntry 	*symlink;

	struct FileTreeEntry 	*parent;
	struct FileTreeEntry 	*children;
	struct FileTreeEntry 	*next;
} FileTreeEntry;

typedef struct {
	FileTreeEntry *FileData;
	FileDescriptorAccessMode AccessMode;
} FileDescriptor;

#endif