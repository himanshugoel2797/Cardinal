/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "cpuid.h"

static uint32_t eax, ebx, ecx, edx;
static uint32_t cache_line_size = 0;

void
CPUID_RequestInfo(uint32_t eax,
                  uint32_t ecx) {
    __asm__ volatile ("cpuid\n\t"
                      : "=a"(eax), "=b"(ebx), "=c" (ecx), "=d" (edx)
                      : "a" (eax), "c"(ecx));
}

uint32_t
CPUID_GetCacheLineSize(void) {
    if(cache_line_size == 0) {
        return 64;
    }
    return cache_line_size;
}

bool
CPUID_FeatureIsAvailable(CPUID_REG cpuid_register,
                         CPUID_FEAT feature) {
    switch(cpuid_register) {
    case CPUID_EAX:
        return (uint32_t)(eax & feature) == (uint32_t)feature;
    case CPUID_EBX:
        return (uint32_t)(ebx & feature) == (uint32_t)feature;
    case CPUID_ECX:
        return (uint32_t)(ecx & feature) == (uint32_t)feature;
    case CPUID_EDX:
        return (uint32_t)(edx & feature) == (uint32_t)feature;
    default:
        return FALSE;
    }
}

uint32_t
CPUID_GetValue(CPUID_REG cpuid_register) {
    switch(cpuid_register) {
    case CPUID_EAX:
        return eax;
    case CPUID_EBX:
        return ebx;
    case CPUID_ECX:
        return ecx;
    case CPUID_EDX:
        return edx;
    default:
        return 0;
    }
}