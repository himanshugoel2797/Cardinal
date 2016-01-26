#ifndef _PHYS_MEM_MANAGER_H_
#define _PHYS_MEM_MANAGER_H_

#include "types.h"

uint32_t
MemMan_Initialize(void);

uint32_t
MemMan_AllocAddress(uint64_t 	addr,
		    size_t 	size);

#endif /* end of include guard: _PHYS_MEM_MANAGER_H_ */
