#ifndef _CARDINAL_IO_H_
#define _CARDINAL_IO_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "cardinal_types.h"

struct StatData {
    uint64_t dev_id;
    uint64_t unique_id;
    uint64_t mode;
    uint64_t type;
    uint32_t link_cnt;
    UID uid;
    UID pid;    //Owner Server PID
    uint64_t size;
    uint32_t pref_block_size;
};

struct FS_StatData {
	uint64_t rsv[32];
};

struct DirEntryData {
    uint64_t unique_id;
    uint64_t offset;
    uint16_t len;
    uint8_t type;
    char    name[1];
};

#endif