#include <stdlib.h>
#include <string.h>

#include "mount_db.h"


typedef struct {
    int flags;
    int mode;
    uint64_t fd;
    uint64_t hash;
    FileSystemObject *obj;
} FileDescriptor;

static List *fds;
static uint64_t fd_base = 0;
static uint32_t fd_cnt_limit = FD_DEFAULT_LIMIT;

static FileSystemObject *root;
static char root_path[PATH_MAX];

int
strcmp_path(const char *a, const char *b) {

    const char *a0 = (strlen(a) < strlen(b))?a:b;	//a0 is the shorter string
    const char *a1 = (strlen(a) < strlen(b))?b:a;	//a1 is the longer string

    for(int i = 0; i < strlen(a0); i++) {
        if(a0[i] != a1[i])
            return 0;
    }

    return 1;
}

int canonicalize_path(const char *src_path, int src_len, char *dst_path, int dst_len)
{
    char *s_p = src_path;
    char *d_p = dst_path;
    
    if(*s_p != '/')
        return -1;
    
    while(s_p < src_path + src_len && d_p < dst_path + dst_len)
    {
            if(strlen(s_p) >= 2 && (!strncmp(s_p, "//", 2) | (s_p > src_path && !strncmp(s_p - 1, "//", 2))))
            {
                s_p++;
            }
            
            if(strlen(s_p) >= 2 && (!strncmp(s_p - 1, "/./", 3))){
                s_p += 1;
            }else if(strlen(s_p) >= 3 && (!strncmp(s_p - 1, "/../", 4)))
            {
                d_p-=2;
                while(d_p >= dst_path && *d_p != '/')d_p--;
                if(d_p < dst_path)return -1;
                d_p++;
                s_p += 2;
            }else{
                *d_p = *s_p;
                d_p++;
            }
  
        s_p++;
    }
    
    if(s_p < src_path + src_len)return -1;
    return 0;
}

void
SetFDLimit(uint32_t limit) {
    if(limit == 0)limit = FD_DEFAULT_LIMIT;
    fd_cnt_limit = limit;
}

uint32_t
GetFDLimit(void) {
    return fd_cnt_limit;
}

void
InitializeDB(void) {

    fds = List_Create();

    root = malloc(sizeof(FileSystemObject));

    root->ObjectType = FileSystemObjectType_Directory;
    root->Name[0] = 0;
    root->Children = List_Create();
}

FileSystemObject*
ParsePath(char *path_arg) {
    FileSystemObject *r = root;

    char path[PATH_MAX];
    
    if(canonicalize_path(path_arg, path, PATH_MAX) != 0)
        return NULL;

    path += strlen(root_path);

    if(strlen(path) == 0)
        return r;

    if(path[0] != '/')
        return NULL;


    while(path[0] != 0) {
        path++;

        FileSystemObject *b = r;
        uint64_t i = 0;
        for(; i < List_Length(r->Children); i++) {

            FileSystemObject *a = (FileSystemObject*)List_EntryAt(r->Children, i);

            if(strcmp_path(a->Name, path) && (path[strlen(a->Name)] == '/' || path[strlen(a->Name)] == 0)) {
                r = a;
                path += strlen(a->Name);
                break;
            }
        }
        if(i == List_Length(b->Children))
            return NULL;

        //Only paths that traverse mount points can be incomplete
        if(r->ObjectType == FileSystemObjectType_MountPoint)
            break;
    }

    return r;
}

FileSystemError
SetupEntry(char *path, FileSystemObject **obj, char **name) {
    //Extract the directory name
    size_t path_len = strnlen(path, PATH_MAX) + 1;
    char *path_c = malloc(path_len);
    strncpy(path_c, path, path_len);

    if(path_len >= PATH_MAX)
        return FileSystemError_PathTooLong;

    char *offset = NULL;

    for(size_t i = path_len - 1; i >= 0; i--) {
        if(path_c[i] == '/') {
            offset = &path_c[i + 1];

            if(strlen(offset) >= NAME_MAX)
                return FileSystemError_NameTooLong;

            path_c[i] = 0;
            break;
        }
    }
    if(offset == NULL)
        return FileSystemError_PathInvalid;

    FileSystemObject* r = ParsePath(path_c);
    if(r == NULL)
        return FileSystemError_PathDoesNotExist;

    free(path_c);

    if(obj != NULL)*obj = r;
    if(name != NULL)*name = path + (offset - path_c);

    return FileSystemError_None;
}

FileSystemError
CreateDirectory(char *path) {

    char *offset = NULL;
    FileSystemObject *r = NULL;
    FileSystemError err = SetupEntry(path, &r, &offset);
    if(err != FileSystemError_None)
        return err;

    if(r->ObjectType != FileSystemObjectType_Directory)
        return FileSystemError_PathInvalid;

    FileSystemObject *dir = malloc(sizeof(FileSystemObject));
    if(dir == NULL)
        return FileSystemError_AllocationFailed;

    dir->ObjectType = FileSystemObjectType_Directory;
    strcpy(dir->Name, offset);
    dir->Children = List_Create();
    dir->Parent = r;

    dir->ReferenceCount = 1;
    r->ReferenceCount++;

    if(List_AddEntry(r->Children, dir) != ListError_None)
        return free(dir), FileSystemError_AllocationFailed;

    return FileSystemError_None;
}

FileSystemError
CreateFile(char *path, FileHandlers *handlers) {

    char *offset = NULL;
    FileSystemObject *r = NULL;
    FileSystemError err = SetupEntry(path, &r, &offset);
    if(err != FileSystemError_None)
        return err;

    if(r->ObjectType != FileSystemObjectType_Directory)
        return FileSystemError_PathInvalid;

    FileSystemObject *dir = malloc(sizeof(FileSystemObject));
    if(dir == NULL)
        return FileSystemError_AllocationFailed;

    dir->ObjectType = FileSystemObjectType_File;
    strcpy(dir->Name, offset);
    dir->handlers = handlers;
    dir->Parent = r;

    dir->ReferenceCount = 1;
    r->ReferenceCount++;

    if(List_AddEntry(r->Children, dir) != ListError_None)
        return free(dir), FileSystemError_AllocationFailed;

    return FileSystemError_None;
}

FileSystemError
RegisterMount(char *path, uint64_t pid) {

    char *offset = NULL;
    FileSystemObject *r = NULL;
    FileSystemError err = SetupEntry(path, &r, &offset);
    if(err != FileSystemError_None)
        return err;

    if(r->ObjectType != FileSystemObjectType_Directory)
        return FileSystemError_PathInvalid;

    FileSystemObject *dir = malloc(sizeof(FileSystemObject));
    if(dir == NULL)
        return FileSystemError_AllocationFailed;

    dir->ObjectType = FileSystemObjectType_MountPoint;
    strcpy(dir->Name, offset);
    dir->TargetPID = pid;
    dir->Parent = r;

    dir->ReferenceCount = 1;
    r->ReferenceCount++;

    if(List_AddEntry(r->Children, dir) != ListError_None)
        return free(dir), FileSystemError_AllocationFailed;

    return FileSystemError_None;
}

FileSystemError
ConstructPathFromSystemObject(FileSystemObject *obj, char *buf, size_t buf_len) {
    //Construct a path to the root of the filesystem
    if(obj == NULL)
        return FileSystemError_PathInvalid;

    FileSystemObject *tmp = obj;

    memset(buf, 0, buf_len);

    int pos = 0;
    do {
        memmove(buf + pos, buf, strlen(buf));
        buf[0] = '/';
        strcpy(buf + 1, tmp->Name);
        pos += strlen(tmp->Name) + 1;

        if(tmp->Parent == NULL)		//Shouln't ever trigger
            return FileSystemError_PathInvalid;

        tmp = tmp->Parent;
    } while(tmp != root && pos <= buf_len);

    if(pos > buf_len)
        return FileSystemError_PathTooLong;

    return FileSystemError_None;
}


void
SetMountLocation(char *loc) {
    strcpy(root_path, loc);
}

void
GetMountLocation(char *loc, size_t sz) {
    if(sz < strlen(root_path)) {
        *loc = '\0';
        return;
    }

    strcpy(loc, root_path);
}

uint64_t
HashPath(const char *str) {
    uint64_t hash = 0;

    //This may need to be updated to properly account for unicode

    for(int i = 0; i < strlen(str); i++) {
        hash = (hash * 131 + str[i]);
    }

    return hash;
}

uint64_t
AllocateFileDescriptor(int flags, int mode, uint64_t hash, FileSystemObject *m) {

    if(List_Length(fds) >= fd_cnt_limit)
        return false;

    uint64_t fd = ++fd_base;

    FileDescriptor *desc = malloc(sizeof(FileDescriptor));
    if(desc == NULL)
        return -1;

    desc->fd = fd;
    desc->flags = flags;
    desc->mode = mode;
    desc->hash = hash;
    desc->obj = m;

    if(m)m->ReferenceCount++;


    if(List_AddEntry(fds, desc) != ListError_None)
        return free(desc), -1;

    return fd;
}
static bool finder(void *val, void *s_val) {
    FileDescriptor *f_desc = (FileDescriptor*)val;
    if((uint64_t)s_val == f_desc->fd)
        return true;
    return false;
}

bool
GetFileDescriptor(uint64_t fd, int *flags, int *mode, uint64_t *hash, FileSystemObject **a) {

    if(fd > fd_base)
        return false;

    uint64_t index = List_Find(fds, finder, (void*)fd);
    if(index == -1)
        return false;

    FileDescriptor *f_desc = List_EntryAt(fds, index);

    if(flags != NULL)*flags = f_desc->flags;
    if(mode != NULL)*mode = f_desc->mode;
    if(hash != NULL)*hash = f_desc->hash;
    if(a != NULL)*a = f_desc->obj;

    return true;
}

void
FreeFileDescriptor(uint64_t fd) {

    if(fd >= fd_base)
        return;

    uint64_t index = List_Find(fds, finder, (void*)fd);
    if(index == -1)
        return;

    FileDescriptor *f_desc = List_EntryAt(fds, index);
    List_Remove(fds, index);

    if(desc->obj)desc->obj->ReferenceCount--;

    if(desc->obj->ReferenceCount == 0)
        free(desc->obj);

    free(f_desc);

    return;
}

static bool fsobj_finder(void *val, void *s_val) {
    FileSystemObject *f_desc = (FileSystemObject*)val;
    if((uint64_t)s_val == f_desc)
        return true;
    return false;
}

FileSystemError
DeleteFileSystemObject(FileSystemObject *obj) {
    if(obj == NULL)
        return FileSystemError_PathInvalid;

    //Can't delete the root
    if(obj == root)
        return FileSystemError_ReferencesExist;

    //If there is more than 1 reference to the object, it can't be deleted, removing from the tree isn't possible until all file descriptors have been closed,
    //else resolving paths will break, which is still needed.

    if(obj->ReferenceCount != 1)
        return FileSystemError_ReferencesExist;

    if(obj->ObjectType == FileSystemObjectType_Directory && List_Length(obj->Children) != 0)
        return FileSystemError_ReferencesExist;

    //Find the entry in the parent
    uint64_t index = List_Find(obj->Parent->Children, fsobj_finder, obj);

    //Remove the entry
    List_Remove(obj->Parent->Children, index);

    //Delete the entry
    free(obj);

    return
}