#include <stddef.h>
#include <stdint.h>


struct fuse_operations {
	unsigned int flag_nopath : 1;
	unsigned int flag_rsvd : 31;

	int (*getattr)(const char*, struct stat *);
};