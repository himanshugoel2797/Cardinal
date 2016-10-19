#ifndef _CARDINAL_HAL_PROGRAM_H_
#define _CARDINAL_HAL_PROGRAM_H_

#include <stdint.h>
#include <stddef.h>
#include <cardinal/cardinal_types.h>
#include "elf.h"

struct elf_setup_params {
  uint16_t ver;
  uint16_t size;
  uint16_t page_size;
  uint16_t argc;  
  uint64_t rnd_seed;
  uint64_t elf_entry_point;
  char argv[1];
};

int
LoadAndStartApplication(UID pid,
                        void *elf_loc,
                        uint64_t elf_size,
                        const char **argv,
                        uint32_t argc);

#endif