#ifndef _CARDINAL_x86_64_ARCH_DEFS_H_
#define _CARDINAL_x86_64_ARCH_DEFS_H_

#include "types.h"

#define IRQ(n) (n + 32)
#define HALT(x) __asm__ volatile("cli\n\thlt" :: "a"(x))
#define PAUSE __asm__ volatile("pause")

typedef struct {
    uint64_t rsp, r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, useresp, ss;
} __attribute__((packed)) Registers;

#endif