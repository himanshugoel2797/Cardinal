/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_CPUID_H_
#define _CARDINAL_CPUID_H_

#include "types.h"

typedef enum {
    CPUID_FEAT_ECX_SSE3         = 1 << 0,
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,
    CPUID_FEAT_ECX_VMX          = 1 << 5,
    CPUID_FEAT_ECX_SMX          = 1 << 6,
    CPUID_FEAT_ECX_EST          = 1 << 7,
    CPUID_FEAT_ECX_TM2          = 1 << 8,
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13,
    CPUID_FEAT_ECX_ETPRD        = 1 << 14,
    CPUID_FEAT_ECX_PDCM         = 1 << 15,
    CPUID_FEAT_ECX_DCA          = 1 << 18,
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19,
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20,
    CPUID_FEAT_ECX_x2APIC       = 1 << 21,
    CPUID_FEAT_ECX_MOVBE        = 1 << 22,
    CPUID_FEAT_ECX_POPCNT       = 1 << 23,
    CPUID_FEAT_ECX_AES          = 1 << 25,
    CPUID_FEAT_ECX_XSAVE        = 1 << 26,
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27,
    CPUID_FEAT_ECX_AVX          = 1 << 28,

    CPUID_FEAT_EDX_FPU          = 1 << 0,
    CPUID_FEAT_EDX_VME          = 1 << 1,
    CPUID_FEAT_EDX_DE           = 1 << 2,
    CPUID_FEAT_EDX_PSE          = 1 << 3,
    CPUID_FEAT_EDX_TSC          = 1 << 4,
    CPUID_FEAT_EDX_MSR          = 1 << 5,
    CPUID_FEAT_EDX_PAE          = 1 << 6,
    CPUID_FEAT_EDX_MCE          = 1 << 7,
    CPUID_FEAT_EDX_CX8          = 1 << 8,
    CPUID_FEAT_EDX_APIC         = 1 << 9,
    CPUID_FEAT_EDX_SEP          = 1 << 11,
    CPUID_FEAT_EDX_MTRR         = 1 << 12,
    CPUID_FEAT_EDX_PGE          = 1 << 13,
    CPUID_FEAT_EDX_MCA          = 1 << 14,
    CPUID_FEAT_EDX_CMOV         = 1 << 15,
    CPUID_FEAT_EDX_PAT          = 1 << 16,
    CPUID_FEAT_EDX_PSE36        = 1 << 17,
    CPUID_FEAT_EDX_PSN          = 1 << 18,
    CPUID_FEAT_EDX_CLF          = 1 << 19,
    CPUID_FEAT_EDX_DTES         = 1 << 21,
    CPUID_FEAT_EDX_ACPI         = 1 << 22,
    CPUID_FEAT_EDX_MMX          = 1 << 23,
    CPUID_FEAT_EDX_FXSR         = 1 << 24,
    CPUID_FEAT_EDX_SSE          = 1 << 25,
    CPUID_FEAT_EDX_SSE2         = 1 << 26,
    CPUID_FEAT_EDX_SS           = 1 << 27,
    CPUID_FEAT_EDX_HTT          = 1 << 28,
    CPUID_FEAT_EDX_TM1          = 1 << 29,
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
} CPUID_FEAT;

typedef enum {
    CPUID_EAX = 0,
    CPUID_EBX,
    CPUID_ECX,
    CPUID_EDX
} CPUID_REG;

typedef enum {
    CPUID_ECX_IGNORE = 0,
    CPUID_EAX_FIRST_PAGE = 1
} CPUID_REQUESTS;

void
CPUID_RequestInfo(uint32_t eax,
                  uint32_t ecx);

bool
CPUID_FeatureIsAvailable(CPUID_REG cpuid_register,
                         CPUID_FEAT feature);

uint32_t
CPUID_GetValue(CPUID_REG cpuid_register);

uint32_t
CPUID_GetCacheLineSize(void);

#endif