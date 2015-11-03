#ifndef _PHYS_MEM_MANAGER_H_
#define _PHYS_MEM_MANAGER_H_

#include "types.h"

uint32_t 
pmem_Initialize(void);

uint64_t 
physMemMan_Alloc(void);

void 
physMemMan_Free(uint64_t ptr);

void 
physMemMan_MarkUsed(uint64_t addr, 
                    uint64_t size);

#endif /* end of include guard: _PHYS_MEM_MANAGER_H_ */
