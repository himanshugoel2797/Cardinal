#include "phys_mem_manager.h"
#include "managers.h"
#include "memory.h"
#include "bootinfo.h"
#include "boot_information/boot_information.h"
#include "elf.h"
#include "common/common.h"

#define BLOCK_SIZE MiB(256)

typedef struct BlockTree{
  uint64_t addr;
  int page_count;
  struct BlockTree *prev;
  struct BlockTree *next;
}BlockTree;

/*
 * Allocations remove space from the free tree
 * Prioritize and try not to allocate from pci and isa dma pools
 * On allocations, free space is allocated from smaller blocks first
 */

static uint64_t memory_size,
       page_count,
       freePageCount;

static BlockTree *highestPriority, *midPriority, *lowPriority;

extern uint64_t _region_kernel_start_, _region_kernel_end_;
extern uint64_t KERNEL_VADDR;

static uint64_t region_kernel_start, region_kernel_end;

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

    lowPriority = bootstrap_malloc(sizeof(BlockTree));
    if(lowPriority == NULL)return -1;
    lowPriority->addr = 0x100000000;
    lowPriority->page_count = (memory_size - lowPriority->addr)/PAGE_SIZE;
    if(lowPriority->page_count < 0)lowPriority->page_count = 0;


    midPriority = bootstrap_malloc(sizeof(BlockTree));
    if(midPriority == NULL)return -1;
    midPriority->addr = MiB(16);
    midPriority->page_count = (memory_size - midPriority->addr)/PAGE_SIZE;

    if(midPriority->page_count > (int64_t)((GiB(4) - MiB(16))/PAGE_SIZE))
      midPriority->page_count = (GiB(4) - MiB(16))/PAGE_SIZE;

    if(midPriority->page_count < 0)midPriority->page_count = 0;

    highestPriority = bootstrap_malloc(sizeof(BlockTree));
    if(highestPriority == NULL)return -1;
    highestPriority->addr = 0;
    highestPriority->page_count = memory_size/PAGE_SIZE;

    if(highestPriority->page_count > (int64_t)(MiB(16)/PAGE_SIZE))
      highestPriority->page_count = MiB(16)/PAGE_SIZE;


    //Mark all memory that isn't marked as available by the bios as taken
    for(uint32_t i = 0; i < info->cardinalMemMap_len/sizeof(CardinalMemMap); i++)
    {
      if(info->cardinalMemMap[i].type != 1)
	{
	  MemMan_AllocAddress(info->cardinalMemMap[i].addr,
			      info->cardinalMemMap[i].len);
	}
    }

    region_kernel_start = (uint64_t)&_region_kernel_start_;
    region_kernel_end = (uint64_t)&_region_kernel_end_;

    //Mark all memory used by the kernel as used
    MemMan_AllocAddress(region_kernel_start,
			region_kernel_end - region_kernel_start);

    return 0;
}

uint32_t
MemMan_AllocAddress(uint64_t 	addr,
		    size_t 	size)
{
  //Align the address to page boundaries
  if(addr % PAGE_SIZE != 0)
    {
      size += addr % PAGE_SIZE;
      addr -= addr % PAGE_SIZE;
    }

  if(size == PAGE_SIZE)
    {
      //This is the part that actually does the work
      //Determine which heap this address belongs to

      if(addr > GiB(4))
	{
	  //Low priority heap
        }
      else if(addr > MiB(16))
	{
	  //Mid priority heap
	}
      else
	{
	  //High priority heap
	}

    }else{

      //Break the allocation into chunks
      if(size % PAGE_SIZE != 0)
	size = (size + PAGE_SIZE) - (size % PAGE_SIZE);

      for(size_t i = 0; i < size / PAGE_SIZE; i++)
	{
	  MemMan_AllocAddress(addr + i * PAGE_SIZE,
			      PAGE_SIZE);
	}
    }

  return 0;
}
