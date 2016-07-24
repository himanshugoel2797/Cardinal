#include "fuse.h"

int fuse_version(void) {
    return FUSE_VERSION;
}

int fuse_set_signal_handlers(struct fuse_sesion *se) {

}

void fuse_remove_signal_handlers(struct fuse_session *se) {

}


int fuse_main_real(int argc, char *argv[], const struct fuse_operations *op, size_t op_size, void *user_data) {

}