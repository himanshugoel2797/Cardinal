#include <cardinal/cardinal_types.h>
#include <cardinal/shared_memory.h>
#include <cardinal/file_server.h>
#include <cardinal/syscall.h>
#include <cardinal/ipc.h>
#include "fileserver.h"

uint64_t
io_open(const char * path,
        int flags,
        uint64_t mode) {

    CREATE_NEW_MESSAGE_PTR_TYPE(FileSystemOpOpen, op);

}