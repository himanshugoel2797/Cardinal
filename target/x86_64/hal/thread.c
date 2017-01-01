/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "thread.h"
#include "IDT/idt.h"
#include "apic/apic.h"
#include "utils/native.h"
#include "interrupts.h"
#include "common.h"
#include "syscall.h"
#include "synchronization.h"


const uint64_t ARCH_SPECIFIC_SPACE_SIZE = 32;
const uint64_t ARCH_DATA_FS_OFFSET = 0;
const uint64_t ARCH_DATA_GS_OFFSET = 1;
const uint64_t ARCH_DATA_FLAGS_OFFSET = 2;
const uint64_t ARCH_DATA_GS_KERN_OFFSET = 3;

uint64_t
GetRFLAGS(void);

void
SetRFLAGS(uint64_t);

void
SavePreviousThread(ThreadInfo *src) {
    Registers *regs = GetSavedInterruptState();
    if(src != NULL) {
        LockSpinlock(src->lock);
        src->CurrentStack = regs->rsp;
        UnlockSpinlock(src->lock);
    }
}

void
SwitchToThread(ThreadInfo *dst) {

    bool swap = FALSE;

    LockSpinlock(dst->lock);
    uint64_t target_stack = dst->CurrentStack;
    if(target_stack & 0xf)__asm__ volatile("cli\n\thlt");

    Registers *regs = (Registers*)(target_stack - 8);
    if(regs->cs & 3)
        swap = TRUE;

    UnlockSpinlock(dst->lock);

    if(swap)
        __asm__("swapgs");

    __asm__ volatile("movq %0, %%rsp\n\t"
                     "popq %%r15\n\t"
                     "popq %%r14\n\t"
                     "popq %%r13\n\t"
                     "popq %%r12\n\t"
                     "popq %%r11\n\t"
                     "popq %%r10\n\t"
                     "popq %%r9\n\t"
                     "popq %%r8\n\t"
                     "popq %%rdi\n\t"
                     "popq %%rsi\n\t"
                     "popq %%rbp\n\t"
                     "popq %%rdx\n\t"
                     "popq %%rcx\n\t"
                     "popq %%rbx\n\t"
                     "popq %%rax\n\t"
                     "add $16, %%rsp\n\t"
                     "iretq\n\t" :: "r"(target_stack)
                    );
}

void
SetFSBase(void *base) {
    wrmsr(0xC0000100, (uint64_t)base);
}

void
SetGSBase(void *base) {
    wrmsr(0xC0000101, (uint64_t)base);
}

void
SetBG_GSBase(void *base) {
    wrmsr(0xC0000102, (uint64_t)base);
}

void*
GetFSBase(void) {
    return (void*)rdmsr(0xC0000100);
}

void*
GetGSBase(void) {
    return (void*)rdmsr(0xC0000101);
}

void*
GetBG_GSBase(void) {
    return (void*)rdmsr(0xC0000102);
}

void
PerformArchSpecificTaskSave(ThreadInfo *tInfo) {
    uint64_t *data = (uint64_t*)tInfo->ArchSpecificData;

    data[ARCH_DATA_FS_OFFSET] = (uint64_t)GetFSBase();
    data[ARCH_DATA_GS_OFFSET] = (uint64_t)GetBG_GSBase();   //The user GS is in the background while in the kernel.
    data[ARCH_DATA_FLAGS_OFFSET] = (uint64_t)GetRFLAGS();
}

void
SetupArchSpecificData(ThreadInfo *tInfo, CRegisters *regs) {
    uint64_t *data = (uint64_t*)tInfo->ArchSpecificData;

    data[ARCH_DATA_FS_OFFSET] = (uint64_t)regs->tls;
    data[ARCH_DATA_GS_OFFSET] = 0;
    data[ARCH_DATA_FLAGS_OFFSET] = 1 << 9;
}

void
PerformArchSpecificTaskSwitch(ThreadInfo *tInfo) {
    uint64_t *data = (uint64_t*)tInfo->ArchSpecificData;

    SetFSBase((void*)data[ARCH_DATA_FS_OFFSET]);
    SetBG_GSBase((void*)data[ARCH_DATA_GS_OFFSET]); //The user GS is in the background while in the kernel.
    SetRFLAGS(data[ARCH_DATA_FLAGS_OFFSET]);
}

#include "debug_gfx.h"
void
SetupPreemption(void) {
    uint64_t timer_freq = APIC_GetTimerFrequency()/(1000 * 500) * 1000;
    debug_gfx_writeLine("Timer: %d\r\n", (uint32_t)timer_freq);

    //uint64_t timer_freq = 1000000000;
    SetPeriodicPreemptVector(IRQ(0), timer_freq);
    APIC_SetVector(APIC_TIMER, IRQ(0));
    APIC_SetTimerValue(timer_freq);
    APIC_SetTimerMode(APIC_TIMER_PERIODIC);
    __asm__("sti");
    APIC_SetEnableInterrupt(APIC_TIMER, ENABLE);

}

void
ResetPreemption(void) {
    APIC_SetEnableInterrupt(APIC_TIMER, DISABLE);
    APIC_SetTimerValue(APIC_GetTimerFrequency()/(1000 * 500) * 1000);
    APIC_SetEnableInterrupt(APIC_TIMER, ENABLE);
}