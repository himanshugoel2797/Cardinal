#ifndef _CARDINAL_FILE_SERVER_H_
#define _CARDINAL_FILE_SERVER_H_

#include <stddef.h>
#include <stdint.h>

#define MAX_PATH_LEN 4000
#define MAX_BUF_LEN 4000

struct OpenRequest {
	uint64_t flags;
	uint64_t mode;
	uint64_t path_len;
	char path[1];
};

struct CloseRequest {
	uint64_t fd;
};

struct ReadWriteRequest {
	uint64_t fd;
	uint64_t cnt;
	uint64_t buf[1];
};

#endif