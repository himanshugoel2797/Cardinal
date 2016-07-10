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
void kmalloc_init(void);

//! kernel malloc

//! \param size The size in bytes of the memory to allocate
//! \return The virtual address of the allocated space
//! \sa kfree()
void* kmalloc(size_t size);

//! kernel free

//! \param addr The virtual address previously allocated by kmalloc
//! \sa kmalloc()
void kfree(void* addr);

//! Compact the kernel memory allocation table

//! This shouldn't need to be called from outside
//! \sa kmalloc(), kfree()
void kcompact();

/**@}*/

#endif /* end of include guard: _K_MALLOC_H_ */
