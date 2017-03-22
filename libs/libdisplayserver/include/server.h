#ifndef _CARDINAL_LIBDISPLAY_SERVER_H_
#define _CARDINAL_LIBDISPLAY_SERVER_H_

#include <cardinal/cardinal_types.h>

#define MAX_DISPLAY_NAME_LEN 256

typedef struct {
    char name[MAX_DISPLAY_NAME_LEN];
    uint64_t cmd_shmem_key;
    uint32_t cmd_shmem_size;

    uint64_t resp_shmem_key;
    uint32_t resp_shmem_size;
} DisplayInfo;

//TODO: Use IPC to negotiate a connection
// Then use pipes to communicate

#endif