#ifndef _BOOTSTRAP_MEM_POOL_MANAGER_H_
#define _BOOTSTRAP_MEM_POOL_MANAGER_H_

#include "types.h"

#define BOOTSTRAP_MEM_POOL MiB(8)

void*
bootstrap_malloc(size_t size);

#endif /* end of include guard: _BOOTSTRAP_MEM_POOL_MANAGER_H_ */
