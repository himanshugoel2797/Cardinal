/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "debug_gfx.h"
#include "boot_information/boot_information.h"
#include "bootinfo.h"
#include "font.h"
#include "synchronization.h"
#include "utils/native.h"

static Spinlock lock = NULL;
static char trap_str[512];

void
debug_gfx_init(void) {
    lock = CreateBootstrapSpinlock();
}

void
set_trap_str(const char *str) {
    strcpy(trap_str, str);
}

void
debug_handle_trap(void) {
    int i = 0;

    //Output manually because we can't rely on the environment being very stable.
    while(trap_str[i])
        outb(0x3f8, trap_str[i++]);

    outb(0x3f8, '\r');
    outb(0x3f8, '\n');

    __asm__ volatile("cli\n\thlt" :: "a"(__builtin_return_address(0)));
}

void
debug_gfx_writeLine(const char *fmt, va_list vl) {
    LockSpinlock(lock);

    char str[256];
    memset(str, 0, 256);

    vsnprintf(str, fmt, vl);

    int i = 0;

    while(str[i])
        outb(0x3f8, str[i++]);

    UnlockSpinlock(lock);
}
