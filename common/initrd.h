#ifndef _CARDINAL_TAR_H_
#define _CARDINAL_TAR_H_

#include "types.h"

bool
Initrd_GetFile(const char *file,
               void **loc,
               uint64_t *size);

#endif