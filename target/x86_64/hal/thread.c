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


const uint64_t ARCH_SPECIFIC_SPACE_SIZE = 256;
const uint64_t ARCH_DATA_FS_OFFSET = 0;
const uint64_t ARCH_DATA_GS_OFFSET = 1;
const uint64_t ARCH_DATA_FLAGS_OFFSET = 2;
const uint64_t ARCH_DATA_GS_KERN_OFFSET = 3;

const uint64_t ARCH_DATA_RAX = 4;
const uint64_t ARCH_DATA_RBX = 5;
const uint64_t ARCH_DATA_RCX = 6;
const uint64_t ARCH_DATA_RDX = 7;
const uint64_t ARCH_DATA_RBP = 8;
const uint64_t ARCH_DATA_RSI = 9;
const uint64_t ARCH_DATA_RDI = 10;
const uint64_t ARCH_DATA_R08 = 11;
const uint64_t ARCH_DATA_R09 = 12;
const uint64_t ARCH_DATA_R10 = 13;
const uint64_t ARCH_DATA_R11 = 14;
const uint64_t ARCH_DATA_R12 = 15;
const uint64_t ARCH_DATA_R13 = 16;
const uint64_t ARCH_DATA_R14 = 17;
const uint64_t ARCH_DATA_R15 = 18;
const uint64_t ARCH_DATA_RIP = 19;
const uint64_t ARCH_DATA_CS = 20;
const uint64_t ARCH_DATA_RFLAGS = 21;
const uint64_t ARCH_DATA_RSP = 22;
const uint64_t ARCH_DATA_SS = 23;


uint64_t
GetRFLAGS(void);

void
SetRFLAGS(uint64_t);

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

    Registers *regs = GetSavedInterruptState();
    data[ARCH_DATA_R15] = regs->r15;
    data[ARCH_DATA_R14] = regs->r14;
    data[ARCH_DATA_R13] = regs->r13;
    data[ARCH_DATA_R12] = regs->r12;
    data[ARCH_DATA_R11] = regs->r11;
    data[ARCH_DATA_R10] = regs->r10;
    data[ARCH_DATA_R09] = regs->r9;
    data[ARCH_DATA_R08] = regs->r8;
    data[ARCH_DATA_RAX] = regs->rax;
    data[ARCH_DATA_RBX] = regs->rbx;
    data[ARCH_DATA_RCX] = regs->rcx;
    data[ARCH_DATA_RDX] = regs->rdx;
    data[ARCH_DATA_RBP] = regs->rbp;
    data[ARCH_DATA_RSI] = regs->rsi;
    data[ARCH_DATA_RDI] = regs->rdi;
    data[ARCH_DATA_RIP] = regs->rip;
    data[ARCH_DATA_CS] = regs->cs;
    data[ARCH_DATA_RFLAGS] = regs->eflags;
    data[ARCH_DATA_RSP] = regs->useresp;
    data[ARCH_DATA_SS] = regs->ss;
}

void
SetupArchSpecificData(ThreadInfo *tInfo, CRegisters *regs) {
    uint64_t *data = (uint64_t*)tInfo->ArchSpecificData;

    data[ARCH_DATA_FS_OFFSET] = (uint64_t)regs->tls;
    data[ARCH_DATA_GS_OFFSET] = 0;
    data[ARCH_DATA_FLAGS_OFFSET] = 1 << 9;
    data[ARCH_DATA_R15] = regs->r15;
    data[ARCH_DATA_R14] = regs->r14;
    data[ARCH_DATA_R13] = regs->r13;
    data[ARCH_DATA_R12] = regs->r12;
    data[ARCH_DATA_R11] = regs->r11;
    data[ARCH_DATA_R10] = regs->r10;
    data[ARCH_DATA_R09] = regs->r9;
    data[ARCH_DATA_R08] = regs->r8;
    data[ARCH_DATA_RAX] = regs->rax;
    data[ARCH_DATA_RBX] = regs->rbx;
    data[ARCH_DATA_RCX] = regs->rcx;
    data[ARCH_DATA_RDX] = regs->rdx;
    data[ARCH_DATA_RBP] = regs->rbp;
    data[ARCH_DATA_RSI] = regs->rsi;
    data[ARCH_DATA_RDI] = regs->rdi;
    data[ARCH_DATA_RIP] = regs->rip;
    data[ARCH_DATA_CS] = regs->cs;
    data[ARCH_DATA_RFLAGS] = regs->rflags | (1 << 9);
    data[ARCH_DATA_RSP] = regs->rsp;
    data[ARCH_DATA_SS] = regs->ss;
}

void
PerformArchSpecificTaskSwitch(ThreadInfo *tInfo) {
    uint64_t *data = (uint64_t*)tInfo->ArchSpecificData;

    SetFSBase((void*)data[ARCH_DATA_FS_OFFSET]);
    SetBG_GSBase((void*)data[ARCH_DATA_GS_OFFSET]); //The user GS is in the background while in the kernel.
    SetRFLAGS(data[ARCH_DATA_FLAGS_OFFSET]);

    uint64_t cur_stack_frame[32];
    int offset = 32;

    cur_stack_frame[--offset] = data[ARCH_DATA_SS];
    cur_stack_frame[--offset] = data[ARCH_DATA_RSP];
    cur_stack_frame[--offset] = data[ARCH_DATA_RFLAGS];
    cur_stack_frame[--offset] = data[ARCH_DATA_CS];
    cur_stack_frame[--offset] = data[ARCH_DATA_RIP];
    cur_stack_frame[--offset] = data[ARCH_DATA_RAX];
    cur_stack_frame[--offset] = data[ARCH_DATA_RBX];
    cur_stack_frame[--offset] = data[ARCH_DATA_RCX];
    cur_stack_frame[--offset] = data[ARCH_DATA_RDX];
    cur_stack_frame[--offset] = data[ARCH_DATA_RBP];
    cur_stack_frame[--offset] = data[ARCH_DATA_RSI];
    cur_stack_frame[--offset] = data[ARCH_DATA_RDI];
    cur_stack_frame[--offset] = data[ARCH_DATA_R08];
    cur_stack_frame[--offset] = data[ARCH_DATA_R09];
    cur_stack_frame[--offset] = data[ARCH_DATA_R10];
    cur_stack_frame[--offset] = data[ARCH_DATA_R11];
    cur_stack_frame[--offset] = data[ARCH_DATA_R12];
    cur_stack_frame[--offset] = data[ARCH_DATA_R13];
    cur_stack_frame[--offset] = data[ARCH_DATA_R14];
    cur_stack_frame[--offset] = data[ARCH_DATA_R15];

    if(data[ARCH_DATA_CS] & 3)
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
                     "iretq\n\t" :: "r"(&cur_stack_frame[offset])
                    );
}

#include "debug_gfx.h"
void
SetupPreemption(void) {
    uint64_t timer_freq = APIC_GetTimerFrequency()/(1000 * 500) * 1000;

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