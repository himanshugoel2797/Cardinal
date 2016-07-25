#ifndef _CARDINAL_FILE_FD_H_
#define _CARDINAL_FILE_FD_H_

#include "types.h"
#include "synchronization.h"

#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LEN 4096

typedef enum {
    FileTreeEntryType_File,
    FileTreeEntryType_Directory,
    FileTreeEntryType_FIFO,
    FileTreeEntryType_Pipe,
    FileTreeEntryType_Character,
    FileTreeEntryType_Symlink
} FileTreeEntryType;

typedef enum {
    FileTreeEntryFlags_None,
    FileTreeEntryFlags_Locked
} FileTreeEntryFlags;

typedef enum {
    FileDescriptorFlags_None,
    FileDescriptorFlags_CloseOnExec = 2000000,
    FileDescriptorFlags_NonBlocking = 4000,
} FileDescriptorFlags;

typedef enum {
    FileDescriptorAccessMode_Read,
    FileDescriptorAccessMode_Write
} FileDescriptorAccessMode;

typedef enum {
    FileAccessPermission_Read = 1,
    FileAccessPermission_Write = 2,
    FileAccessPermission_Exec = 4
} FileAccessPermission;

typedef enum {
    FilePermissionLevel_World = 0,
    FilePermissionLevel_Group = 3,
    FilePermissionLevel_Owner = 6
} FilePermissionLevel;

static inline uint32_t 
UpdatePermissionMask(uint32_t mask, 
                     FilePermissionLevel perm_level, 
                     FileAccessPermission perms) {
    return (mask & (0x7 << perm_level)) | (perms << perm_level);
}

typedef struct FileTreeEntry {
    char    				Name[MAX_FILENAME_LENGTH];
    uint32_t 				AccessPermissions;
    FileTreeEntryFlags 		Flags;
    FileTreeEntryType 		Type;
    UID                     FileOwnerUID;
    UID 					FileSystemDriverPID;
    UID						Identifier;
    UID                     LockSourcePID;

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

void
FileMan_Initialize(void);


bool
FileMan_CreateEntry(FileTreeEntryType type,
                    FileTreeEntryFlags flags,
                    UID identifier,
                    UID fs_driver,
                    UID owner_id,
                    uint32_t access_perms,
                    const char *path);

#endif