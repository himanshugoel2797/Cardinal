#include "elf.h"
#include "kmalloc.h"
#include "common.h"

ElfLoaderError
VerifyElf(void *loc,
          uint64_t size,
          bool * _64bit,
          ElfLimitations limits) {

    if(size < sizeof(Elf_CommonEhdr))return ElfLoaderError_NotElf;

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

    if (hdr->e_ident[EI_OSABI] != ELFOSABI_GNU && hdr->e_ident[EI_OSABI] != ELFOSABI_NONE)
        return ElfLoaderError_UnknownABI;

    *_64bit = (hdr->e_ident[EI_CLASS] == ELFCLASS64);

    return ElfLoaderError_Success;
}


ElfLoaderError
LoadElf32(void *loc,
          uint64_t size,
          ElfLimitations UNUSED(limits),
          UID pageTable,
          MemoryAllocationsMap **map,
          UID *id) {
    *id = 0;
    pageTable = 0;
    *map = NULL;
    if(size < sizeof(Elf32_Ehdr))return ElfLoaderError_NotElf;

    loc = NULL;
    //Elf32_Ehdr *hdr = (Elf32_Ehdr*)loc;

    //Elf32_Phdr *phdr = (Elf32_Phdr*)(hdr->e_phoff + (uint64_t)loc);
    //Elf32_Shdr *shdr = (Elf32_Shdr*)(hdr->e_shoff + (uint64_t)loc);

    return ElfLoaderError_Success;
}

ElfLoaderError
LoadElf64(void *loc,
          uint64_t size,
          ElfLimitations UNUSED(limits),
          UID pageTable,
          MemoryAllocationsMap **map,
          UID *id) {
    id = NULL;
    if(size < sizeof(Elf64_Ehdr))return ElfLoaderError_NotElf;
    Elf64_Ehdr *hdr = (Elf64_Ehdr*)loc;

    //Elf64_Phdr *phdr = (Elf64_Phdr*)(hdr->e_phoff + (uint64_t)loc);
    Elf64_Shdr *shdr = (Elf64_Shdr*)(hdr->e_shoff + (uint64_t)loc);

    int sh_cnt = hdr->e_shnum;
    for(int i = 0; i < sh_cnt; i++,
            shdr = (Elf64_Shdr*)(hdr->e_shentsize + (uint64_t)shdr)) {
        //Get the section flags
        if(shdr->sh_type == SHT_NULL)continue;
        MemoryAllocationFlags flags = MemoryAllocationFlags_User | MemoryAllocationFlags_Exec | MemoryAllocationFlags_Write;
        if(shdr->sh_flags & SHF_EXECINSTR)flags |= MemoryAllocationFlags_Exec;
        if(shdr->sh_flags & SHF_WRITE)flags |= MemoryAllocationFlags_Write;


        uint64_t sh_addr = (uint64_t)shdr->sh_addr;
        uint64_t sh_size = (uint64_t)shdr->sh_size;
        uint64_t sh_aligned = sh_addr - sh_addr % PAGE_SIZE;
        uint64_t sh_pg_offset = sh_addr % PAGE_SIZE;


        MemoryAllocationsMap *alloc = kmalloc(sizeof(MemoryAllocationsMap));

        uint64_t v_tmp_addr = 0;
        FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                               &v_tmp_addr,
                               PAGE_SIZE,
                               MemoryAllocationType_Heap,
                               MemoryAllocationFlags_Write | MemoryAllocationFlags_Kernel);


        for(uint64_t aligned_addr = sh_aligned; aligned_addr < sh_aligned + sh_size; aligned_addr += PAGE_SIZE) {
            if(GetPhysicalAddressUID(pageTable, (void*)sh_addr) == NULL) {


                uint64_t phys_addr = AllocatePhysicalPage();

                if(aligned_addr > sh_addr) {
                    sh_aligned = aligned_addr;
                    sh_pg_offset = 0;
                }

                MapPage(pageTable,
                        alloc,
                        phys_addr,
                        aligned_addr,
                        PAGE_SIZE,
                        CachingModeWriteBack,
                        MemoryAllocationType_Application,
                        flags
                       );

                if(*map != NULL) {
                    alloc->next = *map;
                    *map = alloc;
                }


                //Get the section type
                switch(shdr->sh_type) {
                case SHT_NULL:
                    break;
                case SHT_PROGBITS:
                    //Setup the mapping and copy over the related data
                    MapPage(GetActiveVirtualMemoryInstance(),
                            NULL,
                            phys_addr,
                            v_tmp_addr,
                            PAGE_SIZE,
                            CachingModeWriteBack,
                            MemoryAllocationType_Heap,
                            MemoryAllocationFlags_Write | MemoryAllocationFlags_Kernel);

                    memcpy((void*)(v_tmp_addr + sh_pg_offset),
                           (uint8_t*)loc + shdr->sh_offset,
                           PAGE_SIZE - sh_pg_offset);

                    UnmapPage(GetActiveVirtualMemoryInstance(),
                              v_tmp_addr,
                              PAGE_SIZE);
                    break;
                case SHT_SYMTAB:
                    break;
                case SHT_STRTAB:
                    break;
                case SHT_RELA:
                    break;
                case SHT_HASH:
                    break;
                case SHT_DYNAMIC:
                    break;
                case SHT_NOTE:
                    break;
                case SHT_NOBITS:
                    //Setup the mapping and clear the related area
                    MapPage(GetActiveVirtualMemoryInstance(),
                            NULL,
                            phys_addr,
                            v_tmp_addr,
                            PAGE_SIZE,
                            CachingModeWriteBack,
                            MemoryAllocationType_Heap,
                            MemoryAllocationFlags_Write | MemoryAllocationFlags_Kernel);

                    memset((void*)(v_tmp_addr + sh_pg_offset),
                           0,
                           PAGE_SIZE - sh_pg_offset);

                    UnmapPage(GetActiveVirtualMemoryInstance(),
                              v_tmp_addr,
                              PAGE_SIZE);
                    break;
                case SHT_REL:
                    break;
                case SHT_SHLIB:
                    break;
                case SHT_DYNSYM:
                    break;
                case SHT_INIT_ARRAY:
                    break;
                case SHT_FINI_ARRAY:
                    break;
                case SHT_PREINIT_ARRAY:
                    break;
                case SHT_GROUP:
                    break;
                case SHT_SYMTAB_SHNDX:
                    break;
                default:
                    break;
                }
            }
        }
    }

    return ElfLoaderError_Success;
}


ElfLoaderError
LoadElf(void *loc,
        uint64_t size,
        ElfLimitations limits,
        UID pageTable,
        MemoryAllocationsMap **map,
        UID *id) {

    bool _64bit = FALSE;
    ElfLoaderError err = VerifyElf(loc, size, &_64bit, limits);
    if (err != ElfLoaderError_Success)return err;

    if(_64bit)
        return LoadElf64(loc, size, limits, pageTable, map, id);
    else
        return LoadElf32(loc, size, limits, pageTable, map, id);

}
