#ifndef _CARDINAL_FUSE_COMMON_H_
#define _CARDINAL_FUSE_COMMON_H_

#include "fuse_opt.h"
#include <stdint.h>
#include <sys/types.h>

#define FUSE_MAJOR_VERSION 3
#define FUSE_MINOR_VERSION 0

#define FUSE_MAKE_VERSION(maj, min) ((maj) * 10 + (min))
#define FUSE_VERSION FUSE_MAKE_VERSION(FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION)

struct fuse_file_info {
	int32_t flags;
	uint32_t writepage : 1;
	uint32_t direct_io : 1;
	uint32_t keep_cache : 1;
	uint32_t flush : 1;
	uint32_t nonseekable : 1;
	uint32_t flock_release : 1;
	uint32_t padding : 27;
	uint64_t fh;
	uint64_t lock_owner;
	uint32_t poll_events;
};

#define FUSE_CAP_ASNC_READ (1 << 0)
#define FUSE_CAP_POSIX_LOCKS (1 << 1)
#define FUSE_CAP_ATOMIC_0_TRUNC (1 << 3)
#define FUSE_CAP_EXPORT_SUPPORT (1 << 4)
#define FUSE_CAP_BIG_WRITES (1 << 5)
#define FUSE_CAP_DONT_MASK (1 << 6)
#define FUSE_CAP_SPLICE_WRITE (1 << 7)
#define FUSE_CAP_SPLICE_MODE (1 << 8)
#define FUSE_CAP_SPLICE_READ (1 << 9)
#define FUSE_CAP_FLOCK_LOCKS (1 << 10)
#define FUSE_CAP_IOCTL_DIR (1 << 11)
#define FUSE_CAP_AUTO_INVAL_DATA (1 << 12)
#define FUSE_CAP_READDIRPLUS (1 << 13)
#define FUSE_CAP_READDIRPLUS_AUTO (1 << 14)
#define FUSE_CAP_ASYNC_DIO (1 << 15)
#define FUSE_CAP_WRITEBACK_CACHE (1 << 16)
#define FUSE_CAP_NO_OPEN_SUPPORT (1 << 17)

struct fuse_conn_info {
	unsigned proto_major;
	unsigned proto_minor;
	unsigned async_read;
	unsigned max_write;
	unsigned max_readahead;
	unsigned capable;
	unsigned want;
	unsigned max_background;
	unsigned congestion_threshold;
	unsigned time_gran;
	unsigned reserved[22];
};

struct fuse_session;
struct fuse_chan;
struct fuse_pollhandle;

int fuse_version(void);

enum fuse_buf_flags {
	FUSE_BUF_IS_FD = (1 << 1),
	FUSE_BUF_FD_SEEK = (1 << 2),
	FUSE_BUF_FD_RETRY = (1 << 3)
};

enum fuse_buf_copy_flags {
	FUSE_BUF_NO_SPLICE = (1 << 1),
	FUSE_BUF_FORCE_SPLICE = (1 << 2),
	FUSE_BUF_SPLICE_MOVE = (1 << 3),
	FUSE_BUF_SPLICE_NONBLOCK = (1 << 4)
};

struct fuse_buf {
	size_t size;
	enum fuse_buf_flags flags;
	void *mem;
	int fd;
	off_t pos;
};

struct fuse_bufvec {
	size_t count;
	size_t idx;
	size_t off;
	struct fuse_buf buf[1];
};

int fuse_set_signal_handlers(struct fuse_sesion *se);

void fuse_remove_signal_handlers(struct fuse_session *se);

#endif