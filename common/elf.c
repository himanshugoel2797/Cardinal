#include "elf.h"

ElfLoaderError
VerifyElf(void *loc,
        uint64_t size,
        bool * _64bit,
        ElfLimitations limits) 
{
    Elf_CommonEhdr *hdr = (Elf_CommonEhdr*) loc;
    if (hdr->e_ident[EI_MAG0] != ELF_MAG0) return ElfLoaderError_NotElf;
    if (hdr->e_ident[EI_MAG1] != ELF_MAG1) return ElfLoaderError_NotElf;
    if (hdr->e_ident[EI_MAG2] != ELF_MAG2) return ElfLoaderError_NotElf;
    if (hdr->e_ident[EI_MAG3] != ELF_MAG3) return ElfLoaderError_NotElf;

    bool arch = limits & 1;
    if ((hdr->e_ident[EI_CLASS] == ELFCLASS32) != arch)
        return ElfLoaderError_UnknownClass;

    arch = (limits >> 1) & 1;
    if ((hdr->e_ident[EI_DATA] == ELFDATA2MSB) != arch)
        return ElfLoaderError_IncompatibleBinary;

    if (hdr->e_type == ET_NONE)
        return ElfLoaderError_NotElf;

    if (hdr->e_machine == ET_NONE)
        return ElfLoaderError_UnknownABI;

    if (hdr->e_version != EV_CURRENT)
        return ElfLoaderError_NotElf;

    if (hdr->e_ident[EI_OSABI] != ELFOSABI_GNU)
        return ElfLoaderError_UnknownABI;

    return ElfLoaderError_Success;
}


ElfLoaderError
LoadElf32(void *loc,
        uint64_t size,
        ElfLimitations limits,
        UID *id)
{
    
}

ElfLoaderError
LoadElf64(void *loc,
        uint64_t size,
        ElfLimitations limits,
        UID *id)
{
    
}


ElfLoaderError
LoadElf(void *loc,
        uint64_t size,
        ElfLimitations limits,
        UID *id) {

    bool _64bit = false;
    ElfLoaderError err = VerifyElf(loc, size, &_64bit, limits);
    if (err != ElfLoaderError_Success)return err;

    if(_64bit)LoadElf64(loc, size, limits, id);
    else LoadElf32(loc, size, limits, id);
            
}
