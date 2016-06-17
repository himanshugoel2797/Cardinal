#include "thread.h"
#include "IDT/idt.h"
#include "utils/native.h"
#include "interrupts.h"
#include "common.h"
#include "synchronization.h"

void
SwitchAndInitializeThread(ThreadInfo *cur_thread) {
    __asm__ volatile
    (
        "mov %%rbx, %%rsp\n\t"
        "popq %%rbx\n\t"
        "popq %%rbx\n\t"
        "popq %%rbx\n\t"
        "popq %%rbx\n\t"
        "pushq %%rax\n\t"
        "ret"
        ::
        "a"(cur_thread->entry_point),
        "b"(cur_thread->stack)
    );
}

void
SavePreviousThread(ThreadInfo *src) {
    Registers *regs = GetSavedInterruptState();
    if(src != NULL){
        LockSpinlock(src->lock);
        src->stack = (void*)regs->rsp;
        UnlockSpinlock(src->lock);
    }
    memset(regs, 0, sizeof(Registers));
}

void
SwitchToThread(ThreadInfo *dst) {
    LockSpinlock(dst->lock);
    uint64_t target_stack = (uint64_t)dst->stack;
    UnlockSpinlock(dst->lock);

    __asm__ volatile("mov %0, %%rsp\n\t"
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
SetTLSBase(void *tlsBase) {
    wrmsr(0xC0000100, (uint64_t)tlsBase);
}