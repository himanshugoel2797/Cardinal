#ifndef _CARDINAL_BOOT_INFO_H_
#define _CARDINAL_BOOT_INFO_H_

#include "cardinal_types.h"

//! Defines the boot information format for Cardinal
typedef struct {
    uint64_t addr;
    uint64_t len;
    uint8_t type;
} CardinalMemMap;

typedef struct {
    uint64_t mem_size;
    uint32_t elf_shdr_type;
    uint32_t elf_shdr_size;
    uint32_t elf_shdr_num;
    uint32_t elf_shdr_entsize;
    uint32_t elf_shdr_shndx;
    uint32_t cardinalMemMap_len;
    uint64_t elf_shdr_addr;
    uint64_t rsdp_addr;
    uint64_t initrd_start_addr;
    uint64_t initrd_len;
    CardinalMemMap *cardinalMemMap;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_red_field_position;
    uint8_t framebuffer_red_mask_size;
    uint8_t framebuffer_green_field_position;
    uint8_t framebuffer_green_mask_size;
    uint8_t framebuffer_blue_field_position;
    uint8_t framebuffer_blue_mask_size;
} CardinalBootInfo;

#endif
