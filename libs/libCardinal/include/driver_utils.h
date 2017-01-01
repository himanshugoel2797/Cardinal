/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_DRIVER_UTILS_H_
#define _CARDINAL_DRIVER_UTILS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_list.h"
#include "syscall_property.h"

#ifdef __x86_64__
static __inline void
outb(const uint16_t port,
     const uint8_t val) {
    __asm__ volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}

static __inline uint8_t
inb(const uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}


static __inline void
outw(const uint16_t port,
     const uint16_t val) {
    __asm__ volatile ("outw %1, %0" : : "dN" (port), "a" (val));
}

static __inline uint16_t
inw(const uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}


static __inline void
outl(const uint16_t port,
     const uint32_t val) {
    __asm__ volatile ("outl %1, %0" : : "dN" (port), "a" (val));
}

static __inline uint32_t
inl(const uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}
#endif

static __inline void
R0_EnterCriticalSection(void) {
#ifdef __x86_64__
    __asm__("cli");
#endif
}

static __inline void
R0_ExitCriticalSection(void) {
#ifdef __x86_64__
    __asm__("sti");
#endif
}

static __inline uint64_t
R01_AllocateInterrupts(uint32_t cnt,
                       uint32_t *val) {
    if(val == NULL)
        return -EINVAL;

    *val = Syscall1(Syscall_R01_AllocateInterrupts, cnt);
    return GetErrno();
}

static __inline uint64_t
R01_RegisterForInterrupts(uint32_t irq,
                          uint32_t cnt) {
    Syscall2(Syscall_R01_RegisterForInterrupts, irq, cnt);
    return GetErrno();
}

static __inline uint64_t
R01_GetIOPrivileges(void) {
#ifdef __x86_64__
    SetProperty(CardinalProperty_IOPL, 0, 3);
    return GetErrno();
#endif
    return -ENOSYS;
}

#endif