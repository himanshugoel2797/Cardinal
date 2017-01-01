/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_PRIV_FUSE_H_
#define _CARDINAL_PRIV_FUSE_H_

#include "fuse.h"
#include "fuse_lowlevel.h"

struct fuse_chan;
struct fuse_11;

struct fuse_session {
    struct fuse_11 *f;
    volatile int exited;
    struct fuse_chain *ch;
};

struct fuse_chan {
    struct fuse_session *se;

    pthread_mutex_t lock;
    int ctr;
    int fd;
};

struct fuse_req {
    struct fuse_11 *f;
    uint64_t unique;
    int ctr;
    pthread_mutex_t lock;
    struct fuse_ctx ctx;
    struct fuse_chan *ch;
    int interrupted;
    unsigned int ioctl_64bit : 1;
    union {
        struct {
            uint64_t unique;
        } i;
        struct {
            fuse_interrupt_func_t func;
            void *data;
        } ni;
    } u;
    struct fuse_req *next;
    struct fuse_req *prev;
};

struct fuse_notify_req {
    uint64_t unique;
    void (*reply)(struct fuse_notify_req*, fuse_req_t, fuse_ino_t, const void*, const struct fuse_buf*);
    struct fuse_notify_req *next;
    struct fuse_notify_req *prev;
};



#endif