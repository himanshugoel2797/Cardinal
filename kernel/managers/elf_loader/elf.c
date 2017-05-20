/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "elf.h"
#include "common.h"

ElfLoaderError VerifyElf(void *loc, uint64_t size, bool *_64bit,
                         ElfLimitations limits) {
  if (size < sizeof(Elf_CommonEhdr)) return ElfLoaderError_NotElf;

  Elf_CommonEhdr *hdr = (Elf_CommonEhdr *)loc;
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

  if (hdr->e_type == ET_NONE) return ElfLoaderError_NotElf;

  if (hdr->e_machine == ET_NONE) return ElfLoaderError_UnknownABI;

  if (hdr->e_version != EV_CURRENT) return ElfLoaderError_NotElf;

  if (hdr->e_ident[EI_OSABI] != ELFOSABI_GNU &&
      hdr->e_ident[EI_OSABI] != ELFOSABI_NONE)
    return ElfLoaderError_UnknownABI;

  *_64bit = (hdr->e_ident[EI_CLASS] == ELFCLASS64);

  return ElfLoaderError_Success;
}

ElfLoaderError LoadElf64(void *loc, uint64_t size, ElfLimitations limits,
                         UID pid) {
  if (size < sizeof(Elf64_Ehdr)) return ElfLoaderError_NotElf;
  Elf64_Ehdr *hdr = (Elf64_Ehdr *)loc;

  limits = 0;

  ProcessInfo *pInfo = NULL;
  if (GetProcessReference(pid, &pInfo) != ThreadError_None)
    return ElfLoaderError_UIDNotFound;

  ProcessInfo *curInfo = NULL;
  if (GetProcessReference(GetCurrentProcessUID(), &curInfo) !=
      ThreadError_None) {
    ReturnProcessReference(GetCurrentProcessUID());
    return ElfLoaderError_UIDNotFound;
  }

  Elf64_Phdr *phdr = (Elf64_Phdr *)(hdr->e_phoff + (uint64_t)loc);
  for (int i = 0; i < (int)hdr->e_phnum;
       i++, phdr = (Elf64_Phdr *)((uint64_t)phdr + hdr->e_phentsize)) {
    if (phdr->p_type != PT_LOAD) continue;

    MemoryAllocationFlags flags =
        MemoryAllocationFlags_User | MemoryAllocationFlags_Present;
    if (phdr->p_flags & PF_X) flags |= MemoryAllocationFlags_Exec;
    if (phdr->p_flags & PF_W) flags |= MemoryAllocationFlags_Write;

    uint32_t page_count =
        (phdr->p_vaddr % PAGE_SIZE + phdr->p_memsz) / PAGE_SIZE;
    if (page_count * PAGE_SIZE < (phdr->p_vaddr % PAGE_SIZE + phdr->p_memsz))
      page_count++;

    uint64_t p_addr = 0;
    uint64_t p_vaddr = phdr->p_vaddr;
    uint64_t p_memsz = phdr->p_memsz;
    uint64_t p_filesz = phdr->p_filesz;
    uint64_t p_offset = phdr->p_offset;

    for (uint32_t j = 0; j < page_count; j++) {
      uint64_t mem_clr_sz = p_memsz > (PAGE_SIZE - p_vaddr % PAGE_SIZE)
                                ? (PAGE_SIZE - p_vaddr % PAGE_SIZE)
                                : p_memsz;
      uint64_t mem_cpy_sz = p_filesz > (PAGE_SIZE - p_vaddr % PAGE_SIZE)
                                ? (PAGE_SIZE - p_vaddr % PAGE_SIZE)
                                : p_filesz;

      p_addr = AllocatePhysicalPage();

      uint64_t vaddr = p_vaddr - p_vaddr % PAGE_SIZE;
      if (MapPage(pInfo->PageTable, p_addr, vaddr, PAGE_SIZE,
                  CachingModeWriteBack, MemoryAllocationType_Application,
                  flags) != MemoryAllocationErrors_None) {
        // TODO: handle memory leak
        ReturnProcessReference(GetCurrentProcessUID());
        ReturnProcessReference(pid);
      }

      uint64_t v_tmp_addr =
          SetupTemporaryWriteMap(pInfo->PageTable, vaddr, PAGE_SIZE);

      switch (phdr->p_type) {
        case PT_LOAD:
          memset((void *)v_tmp_addr + p_vaddr % PAGE_SIZE, 0, mem_clr_sz);
          memcpy((void *)v_tmp_addr + p_vaddr % PAGE_SIZE,
                 (uint8_t *)loc + p_offset, mem_cpy_sz);
          break;
      }

      UninstallTemporaryWriteMap(v_tmp_addr, PAGE_SIZE);

      p_memsz -= mem_clr_sz;
      p_filesz -= mem_cpy_sz;
      p_vaddr += (mem_cpy_sz > mem_clr_sz) ? mem_cpy_sz : mem_clr_sz;
      p_offset += mem_cpy_sz;
    }
  }

  ReturnProcessReference(GetCurrentProcessUID());
  ReturnProcessReference(pid);

  return ElfLoaderError_Success;
}

ElfLoaderError LoadElf(void *loc, uint64_t size, ElfLimitations limits,
                       UID pid) {
  bool _64bit = FALSE;
  ElfLoaderError err = VerifyElf(loc, size, &_64bit, limits);
  if (err != ElfLoaderError_Success) return err;

  if (_64bit)
    return LoadElf64(loc, size, limits, pid);
  else
    return ElfLoaderError_IncompatibleBinary;
}

ElfLoaderError ReadElf64(void *loc, uint64_t size, ElfLimitations limits,
                         ElfInformation *elfData) {
  if (size < sizeof(Elf64_Ehdr)) return ElfLoaderError_NotElf;
  Elf64_Ehdr *hdr = (Elf64_Ehdr *)loc;

  limits = 0;

  elfData->entry_point = (void *)hdr->e_entry;
  elfData->phdr_data = (void *)(hdr->e_phoff + (uint64_t)loc);
  elfData->phdr_num = hdr->e_phnum;
  elfData->phdr_ent_size = hdr->e_phentsize;

  return ElfLoaderError_Success;
}

ElfLoaderError ReadElf(void *loc, uint64_t size, ElfLimitations limits,
                       ElfInformation *elfData) {
  bool _64bit = FALSE;
  ElfLoaderError err = VerifyElf(loc, size, &_64bit, limits);
  if (err != ElfLoaderError_Success) return err;

  if (_64bit)
    return ReadElf64(loc, size, limits, elfData);
  else
    return ElfLoaderError_IncompatibleBinary;
}