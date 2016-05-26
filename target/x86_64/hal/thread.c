#include "thread.h"


void
SwitchAndInitializeThread(ThreadInfo *cur_thread) {
    __asm__ volatile
    (
        "mov %%rbx, %%rsp\n\t"
        "pushq %%rax\n\t"
        "ret"
        ::
        "a"(cur_thread->entry_point),
        "b"(cur_thread->stack)
    );
}

void
SwapThreadOnInterrupt(ThreadInfo *src,
                      ThreadInfo *dst) {
    uint64_t stack_frame = 0;

    __asm__ volatile("mov %%rsp, %%rax\n\t"
                     "mov %0, %%rsp\n\t"    : "=a"(stack_frame): "b"(dst->stack)
                    );
    src->stack = (void*)stack_frame;
}
