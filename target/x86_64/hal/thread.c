#include "thread.h"


void
SwitchAndInitializeThread(ThreadInfo *cur_thread) {
    __asm__ volatile
    (
        "mov %0, %%rax\n\t"
        "mov %1, %%rsp\n\t"
        "pushq %%rax\n\t"
        "ret"
        ::
        "ra"(cur_thread->entry_point),
        "rb"(cur_thread->stack)
    );
}

void
SwapThreadOnInterrupt(ThreadInfo *src,
                      ThreadInfo *dst) {
    uint64_t stack_frame = 0;

    __asm__ volatile("mov %%rsp, %%rax\n\t"
                     "mov %0, %%rsp\n\t"    : "=ra"(stack_frame): "rb"(dst->stack)
                    );
    src->stack = (void*)stack_frame;
}
