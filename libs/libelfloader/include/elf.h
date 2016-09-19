#ifndef _CARDINAL_ELF_H_
#define _CARDINAL_ELF_H_

#include <cardinal/cardinal_types.h>
#include <elf.h>

typedef enum ElfLoaderError {
    ElfLoaderError_Success,
    ElfLoaderError_IncompatibleBinary,
    ElfLoaderError_NotElf,
    ElfLoaderError_UnknownABI,
    ElfLoaderError_UnknownClass
} ElfLoaderError;

typedef enum ElfLimitations {
    ElfLimitations_ArchBits = 1,
    ElfLimitations_32Bit = 1,
    ElfLimitations_64Bit = 0,
    ElfLimitations_LSB = (0 << 1),
    ElfLimitations_MSB = (1 << 1)
} ElfLimitations;

typedef struct ElfInformation {
    void *entry_point;
    void *phdr_data;
    uint64_t phdr_num;
    uint64_t phdr_ent_size;
} ElfInformation;


ElfLoaderError
LoadElf(void *loc,
        uint64_t size,
        ElfLimitations limits,
        ManagedPageTable *pageTable,
        ElfInformation *elfData
       );

#endif