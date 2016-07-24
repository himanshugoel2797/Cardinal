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
    FileTreeEntryType_Character,
    FileTreeEntryType_Symlink
} FileTreeEntryType;

typedef enum {
    FileTreeEntryFlags_None
} FileTreeEntryFlags;

typedef enum {
    FileDescriptorFlags_None,
    FileDescriptorFlags_CloseOnExec = (1 << 0)
} FileDescriptorFlags;

typedef enum {
    FileDescriptorAccessMode_Read,
    FileDescriptorAccessMode_Write
} FileDescriptorAccessMode;

typedef struct FileTreeEntry {
    char    				Name[MAX_FILENAME_LENGTH];
    uint64_t 				AccessPermissions;
    FileTreeEntryFlags 		Flags;
    FileTreeEntryType 		Type;
    UID 					FileSystemDriverPID;
    UID						Identifier;

    Spinlock				ReadLock;
    Spinlock				WriteLock;

    uint32_t 				reference_count;

    struct FileTreeEntry 	*parent;
    struct FileTreeEntry 	*children;
    struct FileTreeEntry 	*next;
} FileTreeEntry;

typedef struct {
    FileTreeEntry *FileData;
    FileDescriptorFlags Flags;
    FileDescriptorAccessMode AccessMode;
    Spinlock    lock;
} FileDescriptor;

int
CreateAnonPipe(FileDescriptorFlags flags, int fd[2]);

#endif