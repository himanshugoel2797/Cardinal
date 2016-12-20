#ifndef _PHYS_MEM_MANAGER_H_
#define _PHYS_MEM_MANAGER_H_

#include "types.h"
#include "libs/libCardinal/include/memory.h"

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
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

void
MemMan_MarkFree(uint64_t addr,
                uint64_t size);

uint64_t
MemMan_Alloc2MiBPage(PhysicalMemoryAllocationFlags flags);

uint64_t
MemMan_Alloc2MiBPageCont(int pageCount,
                         PhysicalMemoryAllocationFlags flags);

uint64_t
MemMan_Alloc4KiBPageCont(int pageCount,
                         PhysicalMemoryAllocationFlags flags);

void
MemMan_FreeCont(uint64_t addr,
                int pageCount);

#endif /* end of include guard: _PHYS_MEM_MANAGER_H_ */
