#include "libs/libCardinal/include/syscall.h"

int _start() {
    uint64_t par = 0;
    Syscall(0, 1, &par, 0, NULL);

    __asm__ volatile(
        "loop:\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "jmp loop\n\t" :::
    );

    return 0;
}
