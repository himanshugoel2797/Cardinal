#include "phys_mem_manager.h"
#include "managers.h"
#include "memory.h"
#include "bootinfo.h"
#include "boot_information/boot_information.h"
#include "elf.h"
#include "common/common.h"

static uint64_t memory_size,
       page_count,
       freePageCount,
       lastNonFullPage;

static uint32_t* KB4_Blocks_Bitmap,
       KB4_Blocks_Count;

static const uint64_t block_size = PAGE_SIZE * 32;

extern uint64_t _region_kernel_start_, _region_kernel_end_;
extern uint64_t KERNEL_VADDR;

uint32_t
MemMan_Initialize(void)
{
    // Determine which parts of memory are already in use by the kernel by reading
    // the symbol table
    CardinalBootInfo *info = GetBootInfo();
    memory_size = info->mem_size;

    // Determine the total number of pages
    freePageCount = page_count = memory_size / PAGE_SIZE;
    lastNonFullPage = 0;

    KB4_Blocks_Count = memory_size / (PAGE_SIZE * 32);
    KB4_Blocks_Bitmap = bootstrap_malloc(KB4_Blocks_Count * sizeof(uint32_t));

    memset(KB4_Blocks_Bitmap, 0, KB4_Blocks_Count * sizeof(uint32_t));

    MemMan_MarkUsed((uint64_t)&_region_kernel_start_ - (uint64_t)&KERNEL_VADDR,
                    (uint64_t)&_region_kernel_end_ - (uint64_t)&_region_kernel_start_ + PAGE_SIZE);

    MemMan_MarkUsed(0, MiB(2));


    return 0;
}

static void
MemMan_SetPageUsed(uint64_t addr)
{
    KB4_Blocks_Bitmap[addr/block_size] |= (1 << (addr/PAGE_SIZE % 32));
}

static void
MemMan_SetPageFree(uint64_t addr)
{
    KB4_Blocks_Bitmap[addr/block_size] &= ~(1 << (addr/PAGE_SIZE % 32));
}

void
MemMan_MarkUsed(uint64_t addr,
                uint64_t size)
{
    if(size == 0)return;

    addr = addr/PAGE_SIZE * PAGE_SIZE;

    for(uint64_t i = 0; i < size/PAGE_SIZE; i++)
        {
            MemMan_SetPageUsed(addr);
            addr += PAGE_SIZE;
        }
}

uint64_t
MemMan_Alloc(void)
{
    if(freePageCount == 0)return -1;

    while(KB4_Blocks_Bitmap[lastNonFullPage] == 0xFFFFFFFF)
        lastNonFullPage = (lastNonFullPage + 1) % page_count;

    uint32_t block = ~KB4_Blocks_Bitmap[lastNonFullPage];
    for(int i = 0; i < 32; i++)
        {
            if((block >> i) & 1)
                {
                    uint64_t addr = lastNonFullPage * block_size + i * PAGE_SIZE;
                    MemMan_SetPageUsed(addr);
                    freePageCount--;

                    return addr;
                }
        }

    return -1;
}

void
MemMan_Free(uint64_t ptr)
{
    ptr = ptr/PAGE_SIZE * PAGE_SIZE;

    MemMan_SetPageFree(ptr);
    freePageCount++;
    lastNonFullPage = ptr/block_size;
}
