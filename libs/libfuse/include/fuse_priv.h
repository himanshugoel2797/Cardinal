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
}

#endif