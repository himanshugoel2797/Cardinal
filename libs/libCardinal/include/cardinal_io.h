/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_IO_H_
#define _CARDINAL_IO_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "cardinal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif