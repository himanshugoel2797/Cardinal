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
        "pushq %%rax\n\t"
        "mov $0, %%rax\n\t"
        "mov $0, %%rbx\n\t"
        "mov $0, %%rcx\n\t"
        "mov $0, %%rdx\n\t"
        "mov $0, %%rsi\n\t"
        "mov $0, %%rdi\n\t"
        "mov $0, %%r8\n\t"
        "mov $0, %%r9\n\t"
        "mov $0, %%r10\n\t"
        "mov $0, %%r11\n\t"
        "mov $0, %%r12\n\t"
        "mov $0, %%r13\n\t"
        "mov $0, %%r14\n\t"
        "mov $0, %%r15\n\t"
        "sti\n\t"
        "retq"
        ::
        "a"(cur_thread->entry_point),
        "b"(cur_thread->kernel_stack_aligned)
    );
}

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
SetTLSBase(void *tlsBase) {
    wrmsr(0xC0000100, (uint64_t)tlsBase);
}


void
ApplyProcessPermissions(ProcessPermissions perms)
{
    Registers *regs = GetThreadCurrentStack(GetCurrentThreadUID());

    if(perms & ProcessPermissions_DeviceAccess)
    {

    }
}