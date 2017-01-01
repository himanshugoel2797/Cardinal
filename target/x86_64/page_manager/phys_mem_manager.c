/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "phys_mem_manager.h"
#include "managers.h"
#include "virt_mem_manager/virt_mem_manager.h"
#include "bootinfo.h"
#include "boot_information/boot_information.h"
#include "common/common.h"

static uint64_t memory_size,
       page_count,
       freePageCount,
       lastNonFullPage;

static uint64_t* KB4_Blocks_Bitmap,
       KB4_Blocks_Count;

static const uint64_t block_size = PAGE_SIZE * 64;
static uint64_t dma_store = 0;

extern uint64_t _region_kernel_start_, _region_kernel_end_,
       _bootstrap_region_start, _bootstrap_region_end,
       _trampoline_region_start, _trampoline_region_end;
extern uint64_t KERNEL_VADDR;

uint32_t
MemMan_Initialize(void) {
    // Determine which parts of memory are already in use by the kernel by reading
    // the symbol table
    CardinalBootInfo *info = GetBootInfo();
    memory_size = info->MemorySize;
    memory_size -= memory_size % PAGE_SIZE;

    // Determine the total number of pages
    freePageCount = page_count = memory_size / PAGE_SIZE;

    if(page_count <= GiB(2)/PAGE_SIZE)lastNonFullPage = (page_count / 64)/2 - 1;
    else lastNonFullPage = GiB(0)/(PAGE_SIZE * 64);

    KB4_Blocks_Count = memory_size / (PAGE_SIZE * 64);
    KB4_Blocks_Bitmap = bootstrap_malloc(KB4_Blocks_Count * sizeof(uint64_t));

    //Mark all memory as being in use
    for(uint64_t i = 0; i < KB4_Blocks_Count; i++)
        KB4_Blocks_Bitmap[i] = (uint64_t)-1;

    //Unmark all the regions specified as being free
    for(uint32_t j = 0; j < info->CardinalMemoryMapLength/sizeof(CardinalMemMap); j++) {
        uint64_t addr = info->CardinalMemoryMap[j].addr;
        addr = addr/PAGE_SIZE * PAGE_SIZE;

        uint64_t len = info->CardinalMemoryMap[j].len;
        if(len % PAGE_SIZE != 0)len = (len/PAGE_SIZE + 1) * PAGE_SIZE;

        if(info->CardinalMemoryMap[j].type == CardinalMemoryRegionType_Free)MemMan_MarkFree(addr, len);
    }

    //Mark important regions that have been preallocated
    MemMan_MarkUsed((uint64_t)VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), (void*)info->FramebufferAddress), info->FramebufferPitch * info->FramebufferHeight);

    MemMan_MarkUsed((uint64_t)VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), KB4_Blocks_Bitmap), KB4_Blocks_Count * sizeof(uint64_t));

    MemMan_MarkUsed((uint64_t)VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), (void*)info->InitrdStartAddress), info->InitrdLength);

    MemMan_MarkUsed((uint64_t)&_region_kernel_start_,
                    (uint64_t)&_region_kernel_end_ - (uint64_t)&_region_kernel_start_ + PAGE_SIZE);

    MemMan_MarkUsed(0, MiB(2));

    dma_store = MiB(256) / block_size;

    return 0;
}

static void
MemMan_SetPageUsed(uint64_t addr) {
    KB4_Blocks_Bitmap[addr/block_size] |= (1 << ((addr % block_size)/PAGE_SIZE));
    freePageCount--;
}

static void
MemMan_SetPageFree(uint64_t addr) {
    KB4_Blocks_Bitmap[addr/block_size] &= ~(1 << ((addr % block_size)/PAGE_SIZE));
    freePageCount++;
}

void
MemMan_MarkUsed(uint64_t addr,
                uint64_t size) {
    if(size == 0)return;

    if(size % PAGE_SIZE != 0)size = (size/PAGE_SIZE + 1) * PAGE_SIZE;

    uint64_t page_cnt = size / PAGE_SIZE;

    for(uint64_t i = 0; page_cnt > 0 && (addr % block_size) != 0; i++) {
        MemMan_SetPageUsed(addr);
        addr += PAGE_SIZE;
        page_cnt--;
    }

    for(uint64_t i = 0; page_cnt >= 64 && (addr % block_size) == 0; i++) {
        KB4_Blocks_Bitmap[addr/block_size] = (uint64_t)-1;
        freePageCount -= 64;
        addr += block_size;
        page_cnt -= 64;
    }

    for(uint64_t i = 0; page_cnt > 0; i++) {
        MemMan_SetPageUsed(addr);
        addr += PAGE_SIZE;
        page_cnt--;

    }
}

void
MemMan_MarkFree(uint64_t addr,
                uint64_t size) {
    if(size == 0)return;
    if(size % PAGE_SIZE != 0)size = (size/PAGE_SIZE + 1) * PAGE_SIZE;

    uint64_t page_cnt = size / PAGE_SIZE;

    for(uint64_t i = 0; page_cnt > 0 && (addr % block_size) != 0; i++) {
        MemMan_SetPageFree(addr);
        addr += PAGE_SIZE;
        page_cnt--;
    }

    for(uint64_t i = 0; page_cnt >= 64 && (addr % block_size) == 0; i++) {
        KB4_Blocks_Bitmap[addr/block_size] = 0;
        freePageCount += 64;
        addr += block_size;
        page_cnt -= 64;
    }

    for(uint64_t i = 0; page_cnt > 0; i++) {
        MemMan_SetPageFree(addr);
        addr += PAGE_SIZE;
        page_cnt--;

    }
}

uint64_t
MemMan_Alloc(void) {
    if(freePageCount == 0)return 0;

    return MemMan_Alloc4KiBPageCont(1, PhysicalMemoryAllocationFlags_None);
}

uint64_t
MemMan_Alloc2MiBPage(PhysicalMemoryAllocationFlags flags) {
    return MemMan_Alloc2MiBPageCont(1, flags);
}

uint64_t
MemMan_Alloc2MiBPageCont(int pageCount,
                         PhysicalMemoryAllocationFlags flags) {
    return MemMan_Alloc4KiBPageCont(MiB(2)/KiB(4) * pageCount, flags);
}

uint64_t
MemMan_Alloc4KiBPageCont(int pageCount,
                         PhysicalMemoryAllocationFlags flags) {
    if(freePageCount == 0)return 0;
    if(pageCount == 0)return 0;

    int score = 0;
    uint64_t addr = 0;
    int b_j = 0;

    uint32_t j_min = 0;
    uint32_t j_max = 0;

    if(flags == PhysicalMemoryAllocationFlags_None) {
        j_min = dma_store;
        j_max = KB4_Blocks_Count;
    } else if(flags == PhysicalMemoryAllocationFlags_32Bit) {
        j_min = 0;
        j_max = dma_store;
    } else
        __asm__("cli\n\thlt");

    for(uint32_t j = j_min; j < j_max; j++) {
        if(score == pageCount)
            break;

        uint64_t block = ~KB4_Blocks_Bitmap[j];
        if(block == 0)
            score = 0;

        if((pageCount - score) >= 64){
            if(block != (uint64_t)-1){
                score = 0;
            }else{
                if(score == 0){
                    b_j = j;
                    addr = j * block_size;
                }
                score += 64;
            }
        }else{
            for(int i = 0; i < 64; i++) {
                if(score == pageCount)
                    break;
            
                if((block >> i) & 1) {
                    if(score == 0) {
                        b_j = j;
                        addr = j * block_size + i * PAGE_SIZE;
                    }
                    score++;
                } else {
                    score = 0;
                }
            }
        }
    }

    if(score != pageCount)return 0;
    else {
        MemMan_MarkUsed(addr, pageCount * PAGE_SIZE);
        memset(VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)addr), 0, pageCount * PAGE_SIZE);
        return addr;
    }
}

void
MemMan_Free(uint64_t ptr) {
    ptr = ptr/PAGE_SIZE * PAGE_SIZE;

    MemMan_SetPageFree(ptr);
    lastNonFullPage = ptr/block_size;
}

void
MemMan_FreeCont(uint64_t ptr,
                int pageCount) {
    MemMan_MarkFree(ptr, pageCount * KiB(4));
}