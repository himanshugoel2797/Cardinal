/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "fpu.h"
#include "interrupts.h"
#include "CPUID/cpuid.h"
#include "x86_64_common.h"

static bool xsave_available = FALSE;
static uint64_t state_area_size;

static void
FPU_SIMDException(uint32_t int_no,
                  uint32_t err_no) {
    if(int_no != 0x13)return;
    err_no = 0;

    __asm__ volatile("cli\n\tmov %0, %%rax\n\thlt" :: "a"((uint64_t)0xB00B1E5));

}

static void
FPU_X87Exception(uint32_t int_no,
                 uint32_t err_no) {
    if(int_no != 0x10)return;
    err_no = 0;
    __asm__ volatile("cli\n\tmov %0, %%rax\n\thlt" :: "a"((uint64_t)0xB00B5));
}

void
FPU_Initialize(void) {
    CPUID_RequestInfo(CPUID_EAX_FIRST_PAGE, CPUID_ECX_IGNORE);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FPU))
        bootstrap_kernel_panic(0xff);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_SSE))
        bootstrap_kernel_panic(0xff);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FXSR))
        bootstrap_kernel_panic(0xff);

    xsave_available = CPUID_FeatureIsAvailable(CPUID_ECX, CPUID_FEAT_ECX_XSAVE);

    uint64_t bitmask = 0;
    //Get the current control register 0 value
    __asm__ volatile ("mov %%cr0, %0"
                      : "=r" (bitmask));

    bitmask &= ~(1 << 2); //Enable the FPU
    bitmask |= (1 << 5); //Enable FPU exceptions
    bitmask |= (1 << 1); //FWAIT should cause a state change

    //Set the control register to the new value
    __asm__ volatile ("mov %0, %%cr0"
                      :: "r" (bitmask));

    //Get the current control register 0 value
    __asm__ volatile ("mov %%cr4, %0"
                      : "=r" (bitmask));

    bitmask |= (1 << 9); //Enable SSE support
    bitmask |= (1 << 10); //Enable #XF exception


    if(xsave_available)
        bitmask |= (1 << 18); //Enable XSAVE

    RegisterInterruptHandler(0x10, FPU_X87Exception);
    RegisterInterruptHandler(0x13, FPU_SIMDException);

    //Set the control register to the new value
    __asm__ volatile ("mov %0, %%cr4"
                      :: "r" (bitmask));

    __asm__ volatile ("fninit");

    if(xsave_available) {
        //Determine the XSave area size, and setup XCR0
        CPUID_RequestInfo(0x0D, 0);
        __asm__ volatile("xsetbv" :: "d"(CPUID_GetValue(CPUID_EDX)), "a"(CPUID_GetValue(CPUID_EAX)), "c"((uint32_t)0));

        CPUID_RequestInfo(0x0D, 1);
        state_area_size = CPUID_GetValue(CPUID_EBX);
    } else {
        state_area_size = 1024;
    }
}



uint64_t
GetFPUStateSize(void) {
    return state_area_size;
}

void
SaveFPUState(void *target) {

    if((uint64_t)target % 64)
        __asm__("cli\n\thlt");

    if(xsave_available)
        __asm__ volatile("xsaveq (%0)" :: "r"(target) : "memory");
    else
        __asm__ volatile("fxsaveq (%0)" :: "r"(target) : "memory");
}

void
RestoreFPUState(void *source) {

    if((uint64_t)source % 64)
        __asm__("cli\n\thlt");

    if(xsave_available)
        __asm__ volatile("xrstorq (%0)" :: "r"(source) : "memory");
    else
        __asm__ volatile("fxrstorq (%0)" :: "r"(source) : "memory");
}

/*
void
FPU_SetLazyStateSaveEnableMode(bool enableMode)
{

}*/