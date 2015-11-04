#include "phys_mem_manager.h"
#include "priv_phys_mem_manager.h"
#include "managers.h"
#include "memory.h"
#include "bootinfo.h"
#include "boot_information/boot_information.h"
#include "elf.h"
#include "common/common.h"

static uint64_t memory_size, page_count;

uint32_t
pmem_Initialize(void)
{

    // Determine which parts of memory are already in use by the kernel by reading
    // the symbol table
    CardinalBootInfo *info = GetBootInfo();
    memory_size = info->mem_size;

    // Determine the total number of pages
    freePageCount = totalPageCount = page_count = memory_size / (uint64_t)PAGE_SIZE;
    lastNonFullPage = 0;

    KB4_Blocks_Count = memory_size / ((uint64_t)KiB(4) * 32);
    KB4_Blocks_Bitmap = bootstrap_malloc(KB4_Blocks_Count * sizeof(uint32_t));

    KB4_Blocks_FreeBitCount_EntryNum = KB4_Blocks_Count / KB4_DIVISOR;
    KB4_Blocks_FreeBitCount = bootstrap_malloc(KB4_Blocks_FreeBitCount_EntryNum * sizeof(uint32_t));

    memset(KB4_Blocks_Bitmap, 0, KB4_Blocks_Count * sizeof(uint32_t));

    for (uint64_t i = 0; i < KB4_Blocks_Count; i++)
        {
            SET_FREE_BITCOUNT(i, 32);
        }

    Elf32_Shdr *hdr = (Elf32_Shdr*)info->elf_shdr_addr;
    for (uint32_t i = 0; i < info->elf_shdr_num; i++)
        {
            // Mark the corresponding pages as in use
            if (hdr->sh_size != 0)
                {
                    physMemMan_MarkUsed(hdr->sh_addr, hdr->sh_size);
                }

            hdr++;
        }


/*    multiboot_memory_map_t *mmap = global_memory_map;
    while ( (uint32_t)mmap < (uint32_t)global_memory_map + global_memory_map_size)
        {
            // Make sure this memory is not freeable
            COM_WriteStr("MMAP ADDR: %x LEN: %x TYPE: %x\r\n", (uint32_t)mmap->addr, (uint32_t)mmap->len, (uint32_t)mmap->type);

            if (mmap->type != 1)
                {
                    physMemMan_MarkUsed((uint32_t)mmap->addr, (uint32_t)mmap->len);
                }
            mmap = (multiboot_memory_map_t *)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
        }
*/
        physMemMan_MarkUsed(0, MiB(1));

    return 0;
}

static uint64_t 
MemMan_CalculateBitmapIndex(uint64_t addr, 
                            size_t blockSize)
{
    addr /= blockSize;
    return addr;
}

void 
MemMan_MarkKB4Used(uint64_t addr, 
                   uint64_t size)
{
    if(size == 0)return;
    uint64_t n_addr = KiB(4) * (addr / KiB(4)); // Make addr page aligned
    size += (addr - n_addr);

    uint64_t base_page = n_addr / (uint64_t)KiB(4);
    uint64_t page_count = size / (uint64_t)KiB(4);

    for (uint64_t i = base_page; i < base_page+page_count; i++)
        {
            if(i%32 == 0 && (i+32) < base_page+page_count)
                {
                    KB4_Blocks_Bitmap[i/32] = -1;
                    SET_FREE_BITCOUNT(i/32, 0);
                    freePageCount -= 32;
                    i+=31;
                }
            else
                {
                    KB4_Blocks_Bitmap[i / 32] = SET_BIT(KB4_Blocks_Bitmap[i / 32], (i % 32));
                    DEC_FREE_BITCOUNT(i / 32);
                    freePageCount--;
                    n_addr += KiB(4);
                }
        }
}

void 
physMemMan_MarkUsed(uint64_t addr, 
                    uint64_t size)
{
    MemMan_MarkKB4Used(addr, size);
}

uint64_t 
physMemMan_Alloc(void)
{

    int b = 0;
    uint64_t i = lastNonFullPage;

    for (; i < KB4_Blocks_Count; i++)
        {
            if (KB4_Blocks_Bitmap[i] < 0xFFFFFFFF)
                {

                    b = find_first_zero(KB4_Blocks_Bitmap[i]);
                    KB4_Blocks_Bitmap[i] = SET_BIT(KB4_Blocks_Bitmap[i], b);
                    break;
                }
        }

    if (i >= KB4_Blocks_Count)
        {
            return 0;
        }

    DEC_FREE_BITCOUNT(i);
    freePageCount--;

    uint64_t f_addr = (((i * 32) + b) * KiB(4));

    while (GET_FREE_BITCOUNT(lastNonFullPage) == 0)
        {
            lastNonFullPage = (lastNonFullPage + 1) % KB4_Blocks_Count;
        }

    return f_addr;
}

void 
physMemMan_Free(uint64_t ptr)
{

    uint64_t addr = (uint64_t)ptr;
    uint64_t n_addr = KiB(4) * (addr / KiB(4)); // Make addr page aligned

    uint64_t base_page = n_addr / (uint64_t)KiB(4);

    for (uint64_t i = base_page; i < base_page + 1; i++)
        {
            KB4_Blocks_Bitmap[i / 32] = CLEAR_BIT(KB4_Blocks_Bitmap[i / 32], (i % 32));

            INC_FREE_BITCOUNT(i / 32);
            freePageCount++;

            uint64_t i0 = i / 32;

            if (lastNonFullPage > i0)
                lastNonFullPage = i0;
        }
}
