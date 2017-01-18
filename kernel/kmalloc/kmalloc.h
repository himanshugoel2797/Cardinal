/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _K_MALLOC_H_
#define _K_MALLOC_H_

#include "types.h"
#include "target/hal/memory.h"

/**
 * \defgroup kmalloc Kernel Memory Allocation
 * @{
 */

//! Initialize the kernel memory allocator

//! Requires that the physical memory manager and the virtual memory manager be initialized too
//! \sa kmalloc(), kfree()
void 
kmalloc_init(void);

//! kernel malloc

//! \param size The size in bytes of the memory to allocate
//! \return The virtual address of the allocated space
//! \sa kfree()
__attribute__((malloc))
void* 
kmalloc(size_t size);

//! kernel free

//! \param addr The virtual address previously allocated by kmalloc
//! \sa kmalloc()
void 
kfree(void* addr);

void* 
AllocateMapping(MemoryAllocationType t, 
				MemoryAllocationFlags flags, 
				size_t size);

void 
FreeMapping(void* mem, 
				 size_t size);

void* 
AllocateMappingCont(MemoryAllocationType t, 
					MemoryAllocationFlags flags, 
					size_t size);

void 
FreeMappingCont(void* mem, 
				size_t size);

/**@}*/

#endif /* end of include guard: _K_MALLOC_H_ */
