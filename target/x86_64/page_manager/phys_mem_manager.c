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
    memory_size -= memory_size % PAGE_SIZE;

    // Determine the total number of pages
    freePageCount = page_count = memory_size / PAGE_SIZE;

    if(page_count <= GiB(2)/PAGE_SIZE)lastNonFullPage = (page_count / 32)/2 - 1;
    else lastNonFullPage = GiB(2)/(PAGE_SIZE * 32) - 1;

    KB4_Blocks_Count = memory_size / (PAGE_SIZE * 32);
    KB4_Blocks_Bitmap = bootstrap_malloc(KB4_Blocks_Count * sizeof(uint32_t));

    memset(KB4_Blocks_Bitmap, 0, KB4_Blocks_Count * sizeof(uint32_t));

    MemMan_MarkUsed(info->framebuffer_addr, info->framebuffer_pitch * info->framebuffer_height);

    MemMan_MarkUsed((uint64_t)&_region_kernel_start_ - (uint64_t)&KERNEL_VADDR,
                    (uint64_t)&_region_kernel_end_ - (uint64_t)&_region_kernel_start_ + PAGE_SIZE);

    for(uint32_t j = 0; j < info->cardinalMemMap_len/sizeof(CardinalMemMap); j++)
    {
      uint64_t addr = info->cardinalMemMap[j].addr;
      addr = addr/PAGE_SIZE * PAGE_SIZE;

      uint64_t len = info->cardinalMemMap[j].len;
      if(len % PAGE_SIZE != 0)len = (len/PAGE_SIZE + 1) * PAGE_SIZE;

      if(info->cardinalMemMap[j].type != 1)MemMan_MarkUsed(addr, len);
    }

    MemMan_MarkUsed(0, MiB(2));

    return 0;
}

static void
MemMan_SetPageUsed(uint64_t addr)
{
    KB4_Blocks_Bitmap[addr/block_size] |= (1 << (addr/PAGE_SIZE % 32));
  freePageCount--;
}

static void
MemMan_SetPageFree(uint64_t addr)
{
    KB4_Blocks_Bitmap[addr/block_size] &= ~(1 << (addr/PAGE_SIZE % 32));
  freePageCount++;
}

void
MemMan_MarkUsed(uint64_t addr,
                uint64_t size)
{
    if(size == 0)return;

    if(size % PAGE_SIZE != 0)size = (size/PAGE_SIZE + 1) * PAGE_SIZE;

    addr = addr/PAGE_SIZE * PAGE_SIZE;

    for(uint64_t i = 0; i < size/PAGE_SIZE; i++)
        {
            MemMan_SetPageUsed(addr);
            addr += PAGE_SIZE;
        }
}

void
MemMan_MarkFree(uint64_t addr,
                uint64_t size)
{
    if(size == 0)return;
    if(size % PAGE_SIZE != 0)size = (size/PAGE_SIZE + 1) * PAGE_SIZE;

    addr = addr/PAGE_SIZE * PAGE_SIZE;

    for(uint64_t i = 0; i < size/PAGE_SIZE; i++)
        {
            MemMan_SetPageFree(addr);
            addr += PAGE_SIZE;
        }
}

uint64_t
MemMan_Alloc(void)
{
    if(freePageCount == 0)return 0;

    while(KB4_Blocks_Bitmap[lastNonFullPage] == 0xFFFFFFFF)
        lastNonFullPage = (lastNonFullPage + 1) % page_count;

    uint32_t block = ~KB4_Blocks_Bitmap[lastNonFullPage];
    for(int i = 0; i < 32; i++)
        {
            if((block >> i) & 1)
                {
                    uint64_t addr = lastNonFullPage * block_size + i * PAGE_SIZE;
                    MemMan_SetPageUsed(addr);
                    return addr;
                }
        }

    return 0;
}

uint64_t
MemMan_Alloc2MiBPage(void)
{
    return MemMan_Alloc4KiBPageCont(MiB(2)/KiB(4));
}

uint64_t
MemMan_Alloc2MiBPageCont(int pageCount)
{
    return MemMan_Alloc4KiBPageCont(MiB(2)/KiB(4) * pageCount);
}

uint64_t
MemMan_Alloc4KiBPageCont(int pageCount)
{
    if(freePageCount == 0)return 0;

    int score = 0;
    uint64_t addr = 0;
    int b_j = 0;


    for(uint32_t j = 0; j < KB4_Blocks_Count; j++)
        {
            uint32_t block = ~KB4_Blocks_Bitmap[j];
            for(int i = 0; i < 32; i++)
                {
                    if(score == pageCount)break;
                    if((block >> i) & 1)
                        {
                            if(score == 0)
                                {
                                    b_j = j;
                                    addr = j * block_size + i * PAGE_SIZE;
                                }
                            score++;
                        }
                    else
                        {
                            score = 0;
                        }
                }
        }

    if(score != pageCount)return 0;
    else
        {
            MemMan_MarkUsed(addr, pageCount * KiB(4));
            return addr;
        }
}

void
MemMan_Free(uint64_t ptr)
{
    ptr = ptr/PAGE_SIZE * PAGE_SIZE;

    MemMan_SetPageFree(ptr);
    lastNonFullPage = ptr/block_size;
}

void
MemMan_FreeCont(uint64_t ptr,
                int pageCount)
{
    MemMan_MarkFree(ptr, pageCount * KiB(4));
}
