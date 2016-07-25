#include "file.h"
#include "kmalloc.h"
#include "unicode.h"
#include "managers.h"
#include "common.h"

#define FILETREEENTRY_POOL_SIZE 4096
static FileTreeEntry *pool;

static FileTreeEntry *root;

const char* 
FileMan_GetBtmLevelName(const char *path) {
    size_t len = strnlen_s(path, MAX_PATH_LEN);

    for(;; len--) {
        if(path[len] == '/') break;
        if(len == 0)break;
    }

    return &path[len];
}

const char* 
FileMan_LowerLevel(const char *path) {
    size_t len = strnlen_s(path, MAX_PATH_LEN);
    
    for(size_t i = 0; i < len; i++) {
        if(path[i] == '/')
            return &path[i + 1];
    }

    return NULL;
}

size_t 
FileMan_GetCurrentLevelLength(const char *path) {
    size_t len = strnlen_s(path, MAX_PATH_LEN);

    for(size_t i = 0; i < len; i++) {
        if(path[i] == '/')
            return i;
    }

    return len;
}

bool 
FileMan_IsPathRelative(const char *path) {
    return !(path[0] == '/');
}

void
FileMan_CleanupPath(char *path) {
    size_t len = strnlen_s(path, MAX_PATH_LEN);
    path[len] = '\0';
}

void
FileMan_Initialize(void) {

    pool = AllocateMapping(sizeof(FileTreeEntry) * FILETREEENTRY_POOL_SIZE);
    
    for(int i = 0; i < FILETREEENTRY_POOL_SIZE; i++)
        pool[i].reference_count = 0;

    root = kmalloc(sizeof(FileTreeEntry));
}

bool
FileMan_CreateEntry(FileTreeEntryType type,
                    FileTreeEntryFlags flags,
                    UID identifier,
                    UID fs_driver,
                    UID owner_id,
                    uint32_t access_perms,
                    const char *path) {

    if(!IsUTF8((uint8_t*)path, MAX_PATH_LEN))
        return FALSE;

    size_t p_len = strnlen_s(path, MAX_PATH_LEN);
    if(p_len == 0)
        return FALSE;

    //Make sure the path is null terminated and not length limited
    if(path[p_len + 1] != 0)
        return FALSE;

    FileTreeEntry *fte = kmalloc(sizeof(FileTreeEntry));

    fte->AccessPermissions = access_perms;
    fte->Type = type;
    fte->FileOwnerUID = owner_id;
    fte->FileSystemDriverPID = fs_driver;
    fte->Identifier = identifier;

    if(flags & FileTreeEntryFlags_Locked)
        fte->LockSourcePID = GetCurrentProcessUID();

    fte->ReadLock = CreateSpinlock();
    fte->WriteLock = CreateSpinlock();

    fte->reference_count = 0;

    //Parse the path to get the filename and the tree
    const char *filename = FileMan_GetBtmLevelName(path);
    strcpy_s(fte->Name, MAX_FILENAME_LENGTH, filename, p_len - (size_t)(filename - path));

    FileTreeEntry *base = root;

    if(FileMan_IsPathRelative(path)){
        base = GetCurrentProcessWorkingDirectory();
    }

    return FALSE;
}