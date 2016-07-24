#include "fuse_opt.h"

int fuse_opt_parse(struct fuse_args *args, void *data, const struct fuse_opt opts[], fuse_opt_proc_t proc) {

}

int fuse_opt_add_opt(char **opts, const char *opt) {

}

int fuse_opt_add_opt_escaped(char **opts, const char *opt) {

}

int fuse_opt_add_arg(struct fuse_args *args, const char *arg) {

}

int fuse_opt_insert_arg(struct fuse_args *args, int pos, const char *arg) {

}

void fuse_opt_free_args(struct fuse_args *args) {

}

int fuse_opt_match(const struct fuse_opt opts[], const char *opt) {

}