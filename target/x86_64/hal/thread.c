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
    LockSpinlock(dst->lock);
    uint64_t target_stack = dst->CurrentStack;
    if(target_stack & 0xf)__asm__ volatile("cli\n\thlt");
    UnlockSpinlock(dst->lock);

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

void*
GetFSBase(void) {
    return (void*)rdmsr(0xC0000100);
}

void*
GetGSBase(void) {
    return (void*)rdmsr(0xC0000101);
}

void
PerformArchSpecificTaskSave(ThreadInfo *tInfo) {
    uint64_t *data = (uint64_t*)tInfo->ArchSpecificData;

    data[ARCH_DATA_FS_OFFSET] = (uint64_t)GetFSBase();
    data[ARCH_DATA_GS_OFFSET] = (uint64_t)GetGSBase();
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
    SetGSBase((void*)data[ARCH_DATA_GS_OFFSET]);
    SetRFLAGS(data[ARCH_DATA_FLAGS_OFFSET]);
}

void
SetupPreemption(void) {
    SetPeriodicPreemptVector(IRQ(1), APIC_GetTimerFrequency()/1000);
    APIC_SetVector(APIC_TIMER, IRQ(1));
    APIC_SetTimerValue(APIC_GetTimerFrequency()/1000);
    APIC_SetTimerMode(APIC_TIMER_PERIODIC);
    __asm__("sti");
    APIC_SetEnableInterrupt(APIC_TIMER, ENABLE);

}

void
ResetPreemption(void) {
    __asm__("cli");
    APIC_SetEnableInterrupt(APIC_TIMER, DISABLE);
    APIC_SetTimerValue(APIC_GetTimerFrequency()/1000);
    APIC_SetEnableInterrupt(APIC_TIMER, ENABLE);
    __asm__("sti");
}