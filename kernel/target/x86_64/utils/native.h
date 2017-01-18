/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _UTILS_NATIVE_H_
#define _UTILS_NATIVE_H_

#include "types.h"

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


static __inline void
wrmsr(uint32_t msr,
      uint64_t val) {
    uint32_t lo = val;
    uint32_t hi = (val >> 32);
    __asm__ volatile ("wrmsr" :: "a" (lo), "d" (hi), "c" (msr));

}

static __inline uint64_t
rdmsr(uint32_t msr) {
    uint32_t lo = 0, hi = 0;
    __asm__ volatile ("rdmsr" : "=a" (lo), "=d" (hi) : "c" (msr));
    return ((uint64_t)hi << 32) | lo;
}


#endif /* end of include guard: _UTILS_NATIVE_H_ */
