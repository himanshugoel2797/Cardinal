/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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