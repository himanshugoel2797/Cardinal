#ifdef MULTIBOOT2
#include "boot_information/boot_information.h"
#include "bootinfo.h"
#include "multiboot2.h"
#include "common.h"

static CardinalBootInfo bootInfo;

typedef struct multiboot_tag_framebuffer multiboot_tag_framebuffer;
typedef struct multiboot_tag_mmap multiboot_tag_mmap;
typedef struct multiboot_tag_new_acpi multiboot_tag_new_acpi;

void
ParseAndSaveBootInformation(void *boot_info)
{
    uint8_t *hdr_8 = (uint8_t*)boot_info;
    uint32_t total_size = *(uint32_t*)boot_info;

    memset(&bootInfo, 0, sizeof(CardinalBootInfo));

    uint32_t s = 0;

    for(uint32_t i = 8; i < total_size;)
        {
            uint32_t val = *(uint32_t*)&hdr_8[i];
            switch(val)
                {
                case MULTIBOOT_TAG_TYPE_MMAP:
                {
                    multiboot_tag_mmap *mmap = (multiboot_tag_mmap*)&hdr_8[i];
                    for(uint32_t j = 0; j < (mmap->size - 16); j+= mmap->entry_size)
                        {
                            multiboot_memory_map_t *mmap_e = (multiboot_memory_map_t*)((uint8_t*)mmap->entries + j);
                            if(mmap_e->type == MULTIBOOT_MEMORY_AVAILABLE)
                                bootInfo.mem_size += mmap_e->len;
                        }
                }
                break;
                case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                {
                    multiboot_tag_framebuffer *framebuffer = (multiboot_tag_framebuffer*)&hdr_8[i];
                    bootInfo.framebuffer_addr = framebuffer->common.framebuffer_addr;
                    bootInfo.framebuffer_pitch = framebuffer->common.framebuffer_pitch;
                    bootInfo.framebuffer_width = framebuffer->common.framebuffer_width;
                    bootInfo.framebuffer_height = framebuffer->common.framebuffer_height;
                    bootInfo.framebuffer_bpp = framebuffer->common.framebuffer_bpp;
                }
                break;
                case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                {
                    multiboot_tag_elf_sections *elf = (multiboot_tag_elf_sections*)&hdr_8[i];
                    bootInfo.elf_shdr_type = elf->type;
                    bootInfo.elf_shdr_size = elf->size;
                    bootInfo.elf_shdr_num = elf->num;
                    bootInfo.elf_shdr_entize = elf->entsize;
                    bootInfo.elf_shdr_shndx = elf->shndx;
                    bootInfo.elf_shdr_addr = (uint64_t)elf->sections;

                }
                break;
                case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                {
                    multiboot_tag_new_acpi *acpi = (multiboot_tag_new_acpi*)&hdr_8[i];
                    bootInfo.rsdp_addr = (uint64_t)acpi->rsdp;
                }
                break;
                case MULTIBOOT_TAG_TYPE_END:
                    //i += 8;	//We're done, exit the loop
                    break;
                }
            s = *(uint32_t*)(&hdr_8[i + 4]);
            if(s % 8)s += 8 - (s % 8);
            i += s;
        }
}

CardinalBootInfo*
GetBootInfo(void)
{
    return &bootInfo;
}


#endif