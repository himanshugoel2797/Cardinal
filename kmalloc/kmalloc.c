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

// Allocate a set of pages and maintain a list of them, each page contains a
// kmalloc_page_hdr header to track how much of the page is in use, kernel
// RAM
// is from virtual addresses 0x20000000 to 0x40000000 without regard for the
// associated physical page address

//Allocate a 256MB pool for the kernel and map it to a free address space
void kmalloc_init() {
#define STORE_SIZE MiB(128)

    //TODO Setup TLB shootdowns
    //0xFFFF800000000000 is the table used for APLS
    alloc_sync = CreateBootstrapSpinlock();
    //Allocate blocks of 4KB and map them to a continuous address space of 256MB
    uint64_t virtBaseAddr_base = 0;
    FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                           &virtBaseAddr_base,
                           STORE_SIZE,
                           MemoryAllocationType_Heap,
                           MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Write);

    size_t size = STORE_SIZE;
    while(size > 0) {
        uint64_t physBaseAddr_base = AllocatePhysicalPage();
        MapPage(GetActiveVirtualMemoryInstance(),
                NULL,
                physBaseAddr_base,
                virtBaseAddr_base,
                KiB(4),
                CachingModeWriteBack,
                MemoryAllocationType_Heap,
                MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Write);

        virtBaseAddr_base += KiB(4);
        size -= KiB(4);
    }
    virtBaseAddr_base -= STORE_SIZE;
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

    Balloc_Initialize();
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

bool retry = FALSE;
void *kmalloc(size_t size) {
    LockSpinlock(&alloc_sync);

    if(size < LARGE_HEAP_BLOCK_SIZE)
    {
        UID a = Balloc_Alloc(size);
        if(a != (UID)-1) return Balloc_GetBaseAddress(a);
    }

    kmalloc_info *a_info = allocation_info;
    while(a_info != NULL && a_info->next != NULL) {
        if(IS_FREE(a_info) && a_info->size >= size) {
            break;
        }
        a_info = a_info->next;
    }


    if(IS_USED(a_info) | (a_info->size < size)) {
        //Compact the allocation info and try again, if failed, return NULL
        UnlockSpinlock(&alloc_sync);
        if(!retry) {
            retry = TRUE;
            //kcompact();
            uint64_t res = (uint64_t)kmalloc(size);
            retry = FALSE;
            return (void*)res;
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
    UnlockSpinlock(&alloc_sync);
    return (void*)addr;
}

void kfree(void *addr) {

    LockSpinlock(&alloc_sync);
    //Find the block that matches the address specified
    UID a = Balloc_GetUID(addr);
    if(a != (UID)-1)
    {
        Balloc_Free(a);
        return;
    }

    kmalloc_info *a_info = allocation_info;
    while(a_info->next != NULL) {
        if(IS_USED(a_info) && a_info->pointer == (uint64_t)addr) {
            break;
        }

        a_info = a_info->next;
    }

    //Mark this block as free
    MARK_FREE(a_info);

    UnlockSpinlock(&alloc_sync);
}
