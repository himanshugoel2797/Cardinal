/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
