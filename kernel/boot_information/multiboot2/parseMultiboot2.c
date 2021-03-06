/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifdef MULTIBOOT2
#include "managers/managers.h"
#include "boot_information/boot_information.h"
#include "bootinfo.h"
#include "multiboot2.h"
#include "common.h"

CardinalBootInfo bootInfo;

typedef struct multiboot_tag_framebuffer multiboot_tag_framebuffer;
typedef struct multiboot_tag_mmap multiboot_tag_mmap;
typedef struct multiboot_tag_new_acpi multiboot_tag_new_acpi;
typedef struct multiboot_tag_elf_sections multiboot_tag_elf_sections;
typedef struct multiboot_tag_module multiboot_tag_module;

void
ParseAndSaveBootInformation(void *boot_info, uint32_t magic) {

    ASSERT(magic == MULTIBOOT_MAGIC, "Multiboot2 magic number check failed.");

    uint8_t *hdr_8 = (uint8_t*)boot_info;
    uint32_t total_size = *(uint32_t*)boot_info;

    memset(&bootInfo, 0, sizeof(CardinalBootInfo));

    uint32_t s = 0;

    for(uint32_t i = 8; i < total_size;) {
        uint32_t val = *(uint32_t*)&hdr_8[i];
        switch(val) {
        case MULTIBOOT_TAG_TYPE_MMAP: {
            multiboot_tag_mmap *mmap = (multiboot_tag_mmap*)&hdr_8[i];
            int entryCount = (mmap->size - 16)/mmap->entry_size;
            CardinalMemMap *map = bootstrap_malloc(sizeof(CardinalMemMap) * entryCount);

            for(uint32_t j = 0; j < (mmap->size - 16); j+= mmap->entry_size) {
                multiboot_memory_map_t *mmap_e = (multiboot_memory_map_t*)((uint8_t*)mmap->entries + j);
                bootInfo.MemorySize = mmap_e->addr + mmap_e->len;

                map[j/mmap->entry_size].addr = mmap_e->addr;
                map[j/mmap->entry_size].len = mmap_e->len;
                map[j/mmap->entry_size].type = (CardinalMemoryRegionType)mmap_e->type;
            }

            bootInfo.CardinalMemoryMap = map;
            bootInfo.CardinalMemoryMapLength = sizeof(CardinalMemMap) * entryCount;
        }
        break;
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
            multiboot_tag_framebuffer *framebuffer = (multiboot_tag_framebuffer*)&hdr_8[i];
            bootInfo.FramebufferAddress = framebuffer->common.FramebufferAddress;
            bootInfo.FramebufferPitch = framebuffer->common.FramebufferPitch;
            bootInfo.FramebufferWidth = framebuffer->common.FramebufferWidth;
            bootInfo.FramebufferHeight = framebuffer->common.FramebufferHeight;
            bootInfo.FramebufferBPP = framebuffer->common.FramebufferBPP;

            bootInfo.FramebufferRedFieldPosition = framebuffer->FramebufferRedFieldPosition;
            bootInfo.FramebufferRedMaskSize = framebuffer->FramebufferRedMaskSize;
            bootInfo.FramebufferGreenFieldPosition = framebuffer->FramebufferGreenFieldPosition;
            bootInfo.FramebufferGreenMaskSize = framebuffer->FramebufferGreenMaskSize;
            bootInfo.FramebufferBlueFieldPosition = framebuffer->FramebufferBlueFieldPosition;
            bootInfo.FramebufferBlueMaskSize = framebuffer->FramebufferBlueMaskSize;
        }
        break;
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
            multiboot_tag_elf_sections *elf = (multiboot_tag_elf_sections*)&hdr_8[i];
            bootInfo.elf_shdr_type = elf->type;
            bootInfo.elf_shdr_size = elf->size;
            bootInfo.elf_shdr_num = elf->num;
            bootInfo.elf_shdr_entsize = elf->entsize;
            bootInfo.elf_shdr_shndx = elf->shndx;
            bootInfo.elf_shdr_addr = (uint64_t)elf->sections;

        }
        break;
        case MULTIBOOT_TAG_TYPE_ACPI_OLD:
        case MULTIBOOT_TAG_TYPE_ACPI_NEW: {
            multiboot_tag_new_acpi *acpi = (multiboot_tag_new_acpi*)&hdr_8[i];
            bootInfo.RSDPAddress = (uint64_t)acpi->rsdp;
        }
        break;
        case MULTIBOOT_TAG_TYPE_MODULE: {
            multiboot_tag_module *module = (multiboot_tag_module*)&hdr_8[i];
            bootInfo.InitrdStartAddress = (uint64_t)module->mod_start;
            bootInfo.InitrdLength = (uint64_t)(module->mod_end - module->mod_start);
        }
        break;
        case MULTIBOOT_TAG_TYPE_END:
            //i += 8;   //We're done, exit the loop
            break;
        }
        s = *(uint32_t*)(&hdr_8[i + 4]);
        if(s % 8)s += 8 - (s % 8);
        i += s;
    }
}

CardinalBootInfo*
GetBootInfo(void) {
    return &bootInfo;
}


#endif
