#ifndef _CARDINAL_HAL_PROGRAM_H_
#define _CARDINAL_HAL_PROGRAM_H_

#include <stdint.h>
#include <stddef.h>
#include <cardinal/cardinal_types.h>
#include "elf.h"

int
LoadAndStartApplication(UID pid,
                        void *elf_loc,
                        uint64_t elf_size,
                        const char *argv,
                        uint32_t argc,
                        uint64_t arg_len);

#endif