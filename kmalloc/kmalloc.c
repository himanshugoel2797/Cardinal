#include "kmalloc.h"
#include "synchronization.h"
#include "common/common.h"
#include "memory.h"
#include "managers.h"
#include "block_alloc.h"

static Spinlock alloc_sync;
static int inited = 0;

//A block is free if the first bit is clear
#define IS_FREE(x) ((x->pointer & 1) == 0)
#define IS_USED(x) ((x->pointer & 1) == 1)

#define MARK_FREE(x) (x->pointer = CLEAR_BIT(x->pointer, 0))
#define MARK_USED(x) (x->pointer = SET_BIT(x->pointer, 0))

#define GET_ADDR(x) (x->pointer & ~1)
#define SET_ADDR(x, val) (x->pointer = val | (x->pointer & 1))


//Allocate a 256MB pool for the kernel and map it to a free address space
void kmalloc_init(void) {

    alloc_sync = CreateBootstrapSpinlock();

    Balloc_Initialize();
    inited = 1;
}

#if defined(DEBUG)
static void* ksetup_instrumentation(void* addr, size_t size) {
    if(addr == NULL)__asm__ volatile("cli\n\thlt" :: "a"(size), "b"(__builtin_return_address(1)));
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
    SpinlockUnlocker unlocker = LockSpinlock(alloc_sync);

    if(!inited) {
        void *val = bootstrap_malloc(size);
        return val;
    }

#if defined(DEBUG)
    size += 8 * 6;

#else
#define ksetup_instrumentation(a, b) a

#endif

    if(size < LARGE_HEAP_BLOCK_SIZE) {
        UID a = Balloc_Alloc(size);
        if(a != (UID)-1) {
            return ksetup_instrumentation(Balloc_GetBaseAddress(a), size);
        }
    } else {
        __asm__("cli\n\thlt" :: "a"(__builtin_return_address(0)));
    }

    return NULL;
}

void kfree(void *addr) {
    SpinlockUnlocker unlocker = LockSpinlock(alloc_sync);

    //Find the block that matches the address specified
    UID a = Balloc_GetUID(addr);
    if(a != (UID)-1) {

#if defined(DEBUG)
        uint64_t *top = (uint64_t*)((uint64_t)addr - 8 * 3);
        if(top[0] != 0xDEADBEEFCAFEBABE)__asm__ volatile("cli\n\thlt" :: "a"(addr), "b"(__builtin_return_address(0)));
#endif

        Balloc_Free(a);
        return;
    }
}

void* AllocateMapping(MemoryAllocationType t, MemoryAllocationFlags flags, size_t size) {
    uint64_t user_stack_base = 0;

    if(size % PAGE_SIZE != 0)
        return NULL;

    FindFreeVirtualAddress(
        GetActiveVirtualMemoryInstance(),
        (uint64_t*)&user_stack_base,
        size,
        t,
        MemoryAllocationFlags_Write | MemoryAllocationFlags_Kernel);

    if(user_stack_base == 0)while(1);

    for(uint64_t vaddr = user_stack_base;
            vaddr < user_stack_base + size;
            vaddr += PAGE_SIZE) {

        MapPage(GetActiveVirtualMemoryInstance(),
                AllocatePhysicalPage(),
                vaddr,
                PAGE_SIZE,
                CachingModeWriteBack,
                t,
                flags
               );
    }

    return (void*)user_stack_base;
}

void FreeMapping(void* mem, size_t size) {

    if(size % PAGE_SIZE != 0)return;

    CachingMode cMode = 0;
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          (uint64_t)mem,
                          &cMode,
                          &cFlags,
                          NULL);

    if(cMode != 0 && cFlags != 0 && (cFlags == (MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Write))) {

        for(uint64_t vaddr = (uint64_t)mem;
                vaddr < (uint64_t)mem + size;
                vaddr += PAGE_SIZE) {

            uint64_t addr = (uint64_t)GetPhysicalAddress((void*)vaddr);
            UnmapPage(GetActiveVirtualMemoryInstance(),
                      vaddr,
                      PAGE_SIZE);

            FreePhysicalPage(addr);
        }
    }
}

void* AllocateMappingCont(MemoryAllocationType t, MemoryAllocationFlags flags, size_t size) {
    uint64_t user_stack_base = 0;

    if(size % PAGE_SIZE != 0)
        return NULL;

    FindFreeVirtualAddress(
        GetActiveVirtualMemoryInstance(),
        (uint64_t*)&user_stack_base,
        size,
        t,
        MemoryAllocationFlags_Write | MemoryAllocationFlags_Kernel);

    if(user_stack_base == 0)while(1);

    MapPage(GetActiveVirtualMemoryInstance(),
            AllocatePhysicalPageCont(size/PAGE_SIZE),
            user_stack_base,
            size,
            CachingModeWriteBack,
            t,
            flags
           );

    return (void*)user_stack_base;
}

void FreeMappingCont(void* mem, size_t size) {

    if(size % PAGE_SIZE != 0)return;

    CachingMode cMode = 0;
    MemoryAllocationFlags cFlags = 0;
    GetAddressPermissions(GetActiveVirtualMemoryInstance(),
                          (uint64_t)mem,
                          &cMode,
                          &cFlags,
                          NULL);

    if(cMode != 0 && cFlags != 0 && (cFlags == (MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Write))) {
        uint64_t addr = (uint64_t)GetPhysicalAddress(mem);
        UnmapPage(GetActiveVirtualMemoryInstance(),
                  (uint64_t)mem,
                  size);

        FreePhysicalPageCont(addr, size / PAGE_SIZE);
    }
}