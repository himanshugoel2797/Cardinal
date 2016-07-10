#include "kmalloc.h"
#include "synchronization.h"
#include "common/common.h"
#include "memory.h"
#include "block_alloc.h"

typedef struct kmalloc_info {
    uint64_t pointer;
    size_t size;
    struct kmalloc_info *next;
} kmalloc_info;


void* k_pages_base_addr;
int max_allocs = 0;
uint32_t free_space = 0;
kmalloc_info *allocation_info = NULL, *next_free_block = NULL;
Spinlock alloc_sync;

//A block is free if the first bit is clear
#define IS_FREE(x) ((x->pointer & 1) == 0)
#define IS_USED(x) ((x->pointer & 1) == 1)

#define MARK_FREE(x) (x->pointer = CLEAR_BIT(x->pointer, 0))
#define MARK_USED(x) (x->pointer = SET_BIT(x->pointer, 0))

#define GET_ADDR(x) (x->pointer & ~1)
#define SET_ADDR(x, val) (x->pointer = val | (x->pointer & 1))


//Allocate a 256MB pool for the kernel and map it to a free address space
void kmalloc_init(void) {
#define STORE_SIZE MiB(16)

    //TODO Setup TLB shootdowns
    alloc_sync = CreateBootstrapSpinlock();
    //Allocate blocks of 4KB and map them to a continuous address space of 256MB
    uint64_t virtBaseAddr_base = 0;
    FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                           &virtBaseAddr_base,
                           STORE_SIZE,
                           MemoryAllocationType_Heap,
                           MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Write);

    uint64_t phys_addr = AllocatePhysicalPageCont(STORE_SIZE/PAGE_SIZE);

    MapPage(GetActiveVirtualMemoryInstance(),
            phys_addr,
            virtBaseAddr_base,
            STORE_SIZE,
            CachingModeWriteBack,
            MemoryAllocationType_Heap,
            MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Write);

    next_free_block = allocation_info = (kmalloc_info*)virtBaseAddr_base;
    k_pages_base_addr = (void*)(virtBaseAddr_base + MiB(1));
    max_allocs = MiB(1)/sizeof(kmalloc_info);
    free_space = STORE_SIZE - MiB(1);

    memset(allocation_info, 0, MiB(1));
    allocation_info->pointer = (uint64_t)k_pages_base_addr;
    allocation_info->size = free_space;
    allocation_info->next = NULL;
    MARK_FREE(allocation_info);

    next_free_block++;

    //Balloc_Initialize();
}

void kcompact() {
    kmalloc_info *a_info = allocation_info;

    while(a_info->next != NULL) {
        while(a_info->next != NULL) {
            if(IS_FREE(a_info)) {
                break;
            }
            a_info = a_info->next;
        }
        if(a_info->next != NULL) {
            //TODO this is a memory leak, need some way to reclaim this memory
            a_info->size += a_info->next->size;
            a_info->next = a_info->next->next;
            a_info = a_info->next;
        }
    }
}

#if defined(DEBUG)
static void* ksetup_instrumentation(void* addr, size_t size) {
    size -= 8 * 6;

    uint64_t* top_addr = (uint64_t*)addr;
    top_addr[0] = 0xDEADBEEFCAFEBABE;
    top_addr[1] = size;
    top_addr[2] = 0xDEADBEEFCAFEBABE;

    uint64_t* bottom_addr = (uint64_t*)((uint64_t)top_addr + 8 * 3 + size);
    bottom_addr[0] = 0xDEADC0DEB00B1EE5;
    bottom_addr[1] = size;
    bottom_addr[2] = 0xDEADC0DEB00B1EE5;

    return (void*)((uint64_t)addr + 24);
}

#endif

bool retry = FALSE;
void *kmalloc(size_t size) {
    LockSpinlock(alloc_sync);
    if(allocation_info == NULL)
    {
        UnlockSpinlock(alloc_sync);
        return NULL;
    }

#if defined(DEBUG)
    size += 8 * 6;

#else
#define ksetup_instrumentation(a, b) a

#endif

    /*if(size < LARGE_HEAP_BLOCK_SIZE) {
        UID a = Balloc_Alloc(size);
        if(a != (UID)-1) {
            UnlockSpinlock(alloc_sync);
            return Balloc_GetBaseAddress(a);
        }
    }*/

    kmalloc_info *a_info = allocation_info;
    while(a_info != NULL && a_info->next != NULL) {
        if(IS_FREE(a_info) && a_info->size >= size) {
            break;
        }
        a_info = a_info->next;
    }


    if(IS_USED(a_info) | (a_info->size < size)) {
        //Compact the allocation info and try again, if failed, return NULL
        UnlockSpinlock(alloc_sync);
        if(!retry) {
            retry = TRUE;
            //kcompact();
            uint64_t res = (uint64_t)kmalloc(size);
            retry = FALSE;
            return ksetup_instrumentation((void*)res, size);
        }
        return (void*)NULL;
    }


    //Allocate this block, mark this one as used, append a new block object at the end that contains the remaining free space
    uint64_t addr = GET_ADDR(a_info);
    size_t freeSize = a_info->size - size;

    //We need to allocate a new info block only if there is free space
    if(freeSize != 0) {
        next_free_block->pointer = addr + size;
        next_free_block->size = freeSize;
        next_free_block->next = a_info->next;
        MARK_FREE(next_free_block);

        a_info->next = next_free_block;
        next_free_block++;

    }
    MARK_USED(a_info);
    a_info->size = size;
    //TODO redesign this to automatically request more space when necessary
    UnlockSpinlock(alloc_sync);
    return ksetup_instrumentation((void*)addr, size);
}

void kfree(void *addr) {
    LockSpinlock(alloc_sync);
    //Find the block that matches the address specified
    /*UID a = Balloc_GetUID(addr);
    if(a != (UID)-1) {
        Balloc_Free(a);
        UnlockSpinlock(alloc_sync);
        return;
    }*/

#if defined(DEBUG)
    uint64_t *top = (uint64_t*)((uint64_t)addr - 8 * 3);
    if(top[0] != 0xDEADBEEFCAFEBABE)__asm__ volatile("add $0x28, %%rsp\n\tpopq %%rbx\n\tcli\n\thlt" :: "a"(addr));
#endif

    kmalloc_info *a_info = allocation_info;
    while(a_info->next != NULL) {
        if(IS_USED(a_info) && a_info->pointer == (uint64_t)addr) {
            break;
        }

        a_info = a_info->next;
    }

    //Mark this block as free
    MARK_FREE(a_info);

    UnlockSpinlock(alloc_sync);
}
