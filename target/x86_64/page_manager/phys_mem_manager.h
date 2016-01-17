#ifndef _PHYS_MEM_MANAGER_H_
#define _PHYS_MEM_MANAGER_H_

#include "types.h"

#define PAGE_SIZE (uint64_t)KiB(4)

uint32_t
MemMan_Initialize(void);

uint64_t
MemMan_Alloc(void);

void
MemMan_Free(uint64_t ptr);

void
MemMan_MarkUsed(uint64_t addr,
                uint64_t size);

#endif /* end of include guard: _PHYS_MEM_MANAGER_H_ */
