#ifndef _CARDINAL_BOOT_INFO_H_
#define _CARDINAL_BOOT_INFO_H_

#include "cardinal_types.h"

//! The physical memory regions
typedef enum {
    CardinalMemoryRegionType_Free = 1,  //!< The region is free
} CardinalMemoryRegionType;

//! An entry in the memory map
typedef struct {
    uint64_t addr;                      //!< The base physical address
    uint64_t len;                       //!< The length, in bytes, of the region
    CardinalMemoryRegionType type;      //!< The type of the region
} CardinalMemMap;

//! Defines the boot information format for Cardinal
typedef struct {
    uint64_t MemorySize;                          //!< The total RAM size.
    uint32_t elf_shdr_type;
    uint32_t elf_shdr_size;
    uint32_t elf_shdr_num;
    uint32_t elf_shdr_entsize;
    uint32_t elf_shdr_shndx;
    uint32_t CardinalMemoryMapLength;                //!< The length of the memory map.
    uint64_t elf_shdr_addr;
    uint64_t RSDPAddress;                         //!< The address of the ACPI RSDP table.
    uint64_t InitrdStartAddress;                 //!< The start virtual address of the initrd.
    uint64_t InitrdLength;                        //!< The length, in bytes, of the initrd.
    CardinalMemMap *CardinalMemoryMap;             //!< The memory map.
    uint64_t FramebufferAddress;                  //!< The address of the bootloader provided display framebuffer.
    uint32_t FramebufferPitch;                 //!< The pith of the bootloader provided framebuffer.
    uint32_t FramebufferWidth;                 //!< The width of the bootloader provided framebuffer.
    uint32_t FramebufferHeight;                //!< The height of the bootloader provided framebuffer.
    uint8_t FramebufferBPP;                    //!< The number of bits per pixel of the bootloader provided framebuffer.
    uint8_t FramebufferRedFieldPosition;     //!< The bit position of the red component of a pixel.
    uint8_t FramebufferRedMaskSize;          //!< The bit count of the red component of a pixel.
    uint8_t FramebufferGreenFieldPosition;   //!< The bit position of the green component of a pixel.
    uint8_t FramebufferGreenMaskSize;        //!< The bit count of the green component of a pixel.
    uint8_t FramebufferBlueFieldPosition;    //!< The bit position of the blue component of a pixel.
    uint8_t FramebufferBlueMaskSize;         //!< The bit count of the blue component of a pixel.
} CardinalBootInfo;

#endif
