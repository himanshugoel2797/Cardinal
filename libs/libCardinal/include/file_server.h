#ifndef _CARDINAL_FILE_SERVER_H_
#define _CARDINAL_FILE_SERVER_H_

#include <stddef.h>
#include <stdint.h>

#define MAX_PATH_LEN 4000
#define MAX_BUF_LEN 4000

struct OpenRequest {
    uint64_t flags;
    uint64_t mode;
    char path[1];
};

struct OpenResponse {
	uint64_t fd;
	uint64_t targetPID;
};

struct CloseRequest {
    uint64_t fd;
};

struct ReadWriteRequest {
    uint64_t fd;
    bool lock;
    uint64_t buf[1];
};

#endif