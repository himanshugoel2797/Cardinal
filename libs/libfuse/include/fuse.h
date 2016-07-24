#ifndef _CARDINAL_FUSE_MAIN_H_
#define _CARDINAL_FUSE_MAIN_H_

#include <stddef.h>
#include <stdint.h>

#include "fuse_common.h"
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/uio.h>

enum fuse_readdir_flags {
    FUSE_READDIR_NONE = 0,
    FUSE_READDIR_PLUS = (1 << 0)
};

enum fuse_fill_dir_flags {
    FUSE_FILL_DIR_NONE = 0,
    FUSE_FILL_DIR_PLUS = (1 << 0)
};

typedef int (*fuse_fill_dir_t)(void*, const char*, const struct stat*, off_t, enum fuse_fill_dir_flags);

struct fuse_operations {
    unsigned int flag_nopath : 1;
    unsigned int flag_rsvd : 31;

    int (*getattr)(const char*, struct stat*);
    int (*readlink) (const char*, char*, size_t);
    int (*mknod)(const char*, mode_t, dev_t);
    int (*mkdir)(const char*, mode_t);
    int (*unlink)(const char*);
    int (*rmdir)(const char*);
    int (*symlink)(const char*, const char*);
    int (*rename)(const char*, const char*, unsigned int);
    int (*link)(const char*, const char*);
    int (*chmod)(const char*, mode_t);
    int (*chown)(const char*, uid_t, gid_t);
    int (*truncate)(const char*, off_t);
    int (*open)(const char*, struct fuse_file_info*);
    int (*read)(const char*, char*, size_t, off_t, struct fuse_file_info*);
    int (*write)(const char*, const char*, size_t, off_t, struct fuse_file_info*);
    int (*statfs)(const char*, struct statvfs*);
    int (*flush)(const char*, struct fuse_file_info*);
    int (*release)(const char*, struct fuse_file_info*);
    int (*fsync)(const char*, int, struct fuse_file_info*);
    int (*setxattr)(const char*, const char*, const char*, size_t, int);
    int (*getxattr)(const char*, const char*, char*, size_t);
    int (*listxattr)(const char*, char*, size_t);
    int (*removexattr)(const char*, const char*);
    int (*opendir)(const char*, struct fuse_file_info*);
    int (*readdir)(const char*, void*, fuse_fill_dir_t, off_t, struct fuse_file_info*, enum fuse_readdir_flags);
    int (*releasedir)(const char*, struct fuse_file_info*);
    int (*fsyncdir)(const char*, int, struct fuse_file_info*);
    void*(*init)(struct fuse_conn_info *conn);
    void (*destroy)(void*);
    int (*access)(const char*, int);
    int (*create)(const char*, mode_t, struct fuse_file_info*);
    int (*ftruncate)(const char*, off_t, struct fuse_file_info*);
    int (*fgetattr)(const char*, struct stat*, struct fuse_file_info*);
    int (*lock)(const char*, struct fuse_file_info*, int cmd, struct flock*);
    int (*utimens)(const char*, const struct timespec tv[2]);
    int (*bmap)(const char*, size_t, uint64_t*);
    //TODO keep or leave ioctl?
    int (*poll)(const char*, struct fuse_file_info*, struct fuse_pollhandle *, unsigned *);
    int (*write_buf)(const char*, struct fuse_bufvec*, off_t, structfuse_file_info*);
    int (*read_buf)(const char*, struct fuse_bufvec **, size_t, off_t, struct fuse_file_info*);
    int (*fallocate)(const char*, int, off_t, off_t, struct fuse_file_info*);

};

struct fuse_context {
    struct fuse *fuse;
    uid_t uid;
    gid_t uid;
    pid_t pid;
    void *private_data;
    mode_t umask;
};

#define fuse_main(argc, argv, op, user_data) fuse_main_real(argc, argv, op, sizeof(*(op)), user_data)


#endif