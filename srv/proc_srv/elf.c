/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "elf.h"

#include <string.h>
#include <cardinal/process.h>

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
LoadElf64(void *loc,
          uint64_t size,
          ElfLimitations limits,
          UID pid,
          ElfInformation *elfData) {
    if(size < sizeof(Elf64_Ehdr))return ElfLoaderError_NotElf;
    Elf64_Ehdr *hdr = (Elf64_Ehdr*)loc;

    elfData->entry_point = (void*)hdr->e_entry;
    elfData->phdr_data = (void*)(hdr->e_phoff + (uint64_t)loc);
    elfData->phdr_num = hdr->e_phnum;
    elfData->phdr_ent_size = hdr->e_phentsize;


    Elf64_Phdr *phdr = (Elf64_Phdr*)(hdr->e_phoff + (uint64_t)loc);
    for(int i = 0; i < (int)hdr->e_phnum; i++, phdr = (Elf64_Phdr*)((uint64_t)phdr + hdr->e_phentsize)) {

        if(phdr->p_type != PT_LOAD)continue;

        MemoryAllocationFlags flags = MemoryAllocationFlags_User | MemoryAllocationFlags_Present;
        if(phdr->p_flags & PF_X)flags |= MemoryAllocationFlags_Exec;
        if(phdr->p_flags & PF_W)flags |= MemoryAllocationFlags_Write;


        uint32_t pages_net_size = (phdr->p_vaddr % PAGE_SIZE + phdr->p_memsz);

        if(pages_net_size % PAGE_SIZE)
            pages_net_size += PAGE_SIZE - pages_net_size % PAGE_SIZE;

        uint32_t page_count = pages_net_size / PAGE_SIZE;

        uint64_t p_addr = 0;
        uint64_t p_vaddr = phdr->p_vaddr;
        uint64_t p_memsz = phdr->p_memsz;
        uint64_t p_filesz = phdr->p_filesz;
        uint64_t p_offset = phdr->p_offset;

        for(uint32_t j = 0; j < page_count; j++) {

            uint64_t mem_clr_sz = p_memsz > (PAGE_SIZE - p_vaddr % PAGE_SIZE)?(PAGE_SIZE - p_vaddr % PAGE_SIZE):p_memsz;
            uint64_t mem_cpy_sz = p_filesz > (PAGE_SIZE - p_vaddr % PAGE_SIZE)?(PAGE_SIZE - p_vaddr % PAGE_SIZE) : p_filesz;

            if(R0_AllocatePages(1, PhysicalMemoryAllocationFlags_None, &p_addr) != 0)
                return ElfLoaderError_OutOfMemory;


            uint64_t vaddr = p_vaddr - p_vaddr % PAGE_SIZE;
            if(R0_Map(pid,
                      p_addr,
                      &vaddr,
                      PAGE_SIZE,
                      CachingModeWriteBack,
                      MemoryAllocationType_Application,
                      flags) != 0)
                return ElfLoaderError_OutOfMemory;

            uint64_t v_tmp_addr = 0;
            if(R0_Map(GetCurrentProcessUID(),
                      p_addr,
                      &v_tmp_addr,
                      PAGE_SIZE,
                      CachingModeWriteBack,
                      MemoryAllocationType_Heap | MemoryAllocationType_Phys,
                      MemoryAllocationFlags_User | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Present))
                return ElfLoaderError_OutOfMemory;


            switch(phdr->p_type) {
            case PT_LOAD:
                memset((void*)v_tmp_addr + p_vaddr % PAGE_SIZE, 0, mem_clr_sz);
                memcpy((void*)v_tmp_addr + p_vaddr % PAGE_SIZE, (uint8_t*)loc + p_offset, mem_cpy_sz);
                break;
            }

            R0_Unmap(GetCurrentProcessUID(),
                     v_tmp_addr,
                     PAGE_SIZE);

            p_memsz -= mem_clr_sz;
            p_filesz -= mem_cpy_sz;
            p_vaddr += (mem_cpy_sz > mem_clr_sz)?mem_cpy_sz : mem_clr_sz;
            p_offset += mem_cpy_sz;
        }
    }

    return ElfLoaderError_Success;
}


ElfLoaderError
LoadElf(void *loc,
        uint64_t size,
        ElfLimitations limits,
        UID pid,
        ElfInformation *elfData) {

    bool _64bit = FALSE;
    ElfLoaderError err = VerifyElf(loc, size, &_64bit, limits);
    if (err != ElfLoaderError_Success)return err;

    if(_64bit)
        return LoadElf64(loc, size, limits, pid, elfData);
    else
        return ElfLoaderError_IncompatibleBinary;

}