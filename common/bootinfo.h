#ifndef _CARDINAL_BOOT_INFO_H_
#define _CARDINAL_BOOT_INFO_H_

#include "types.h"

//! Defines the boot information format for Cardinal
typedef struct
{
    uint64_t mem_size;
    uint32_t elf_shdr_type;
    uint32_t elf_shdr_size;
    uint32_t elf_shdr_num;
    uint32_t elf_shdr_entsize;
    uint32_t elf_shdr_shndx;
    uint64_t elf_shdr_addr;
    uint64_t rsdp_addr;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
} CardinalBootInfo;

#endif