#include "thread.h"
#include "IDT/idt.h"
#include "utils/native.h"
#include "interrupts.h"
#include "common.h"
#include "synchronization.h"

void
SavePreviousThread(ThreadInfo *src) {
    Registers *regs = GetSavedInterruptState();
    if(src != NULL) {
        LockSpinlock(src->lock);
        src->current_stack = regs->rsp;
        UnlockSpinlock(src->lock);
    }
}

void
SwitchToThread(ThreadInfo *dst) {
    LockSpinlock(dst->lock);
    uint64_t target_stack = dst->current_stack;
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
    uint64_t *data = (uint64_t*)tInfo->arch_specific_data;

    data[ARCH_DATA_FS_OFFSET] = (uint64_t)GetFSBase();
    data[ARCH_DATA_GS_OFFSET] = (uint64_t)GetGSBase();
}

void
PerformArchSpecificTaskSwitch(ThreadInfo *tInfo) {
    uint64_t *data = (uint64_t*)tInfo->arch_specific_data;

    SetFSBase((void*)data[ARCH_DATA_FS_OFFSET]);
    SetGSBase((void*)data[ARCH_DATA_GS_OFFSET]);
}