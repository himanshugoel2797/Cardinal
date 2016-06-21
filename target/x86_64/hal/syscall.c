#include "syscall.h"
#include "utils/native.h"
#include "managers.h"
#include "common/common.h"

static uint8_t *k_stack = NULL;
static volatile uint64_t rflags = 0;

__attribute__((naked, noreturn))
void
Syscall_Handler(void) {
    //Setup the environment to move into the OS syscall handler
    __asm__ volatile
    (
        "pushq %%rax\n\t"
        "movq %%rsp, %%rax\n\t"
        "movq k_stack, %%rsp\n\t"
        "pushq %%rax\n\t"
        "pushq %%rcx\n\t"
        "pushq %%rdi\n\t"
        "movq %%rcx, %%rdi\n\t"
        "movq %%r11, rflags\n\t"
        "callq (SyscallReceived)\n\t"
        "popq %%rdi\n\t"
        "popq %%rcx\n\t"
        "popq %%rax\n\t"
        "movq %%rax, %%rsp\n\t"
        "popq %%rax\n\t"
        "sysretq\n\t" :::
    );
}

void
SwitchToUserMode(uint64_t pc, uint64_t sp) {
    __asm__ volatile
    (
        "pushfq\n\t"
        "popq %%r11\n\t"
        "orq $512, %%r11\n\t"
        "cli\n\t"
        "mov %%rax, %%rsp\n\t"
        "mov %%rsp, %%rbp\n\t"
        "sysretq\n\t" :: "a"(sp), "c"(pc)
    );
}

void
SetKernelStack(void* stack) {
    k_stack = stack;
}

void
Syscall_Initialize(void) {
    uint64_t star_val = (0x08ull << 32) | (0x18ull << 48);
    uint64_t lstar = (uint64_t)Syscall_Handler;
    uint64_t cstar = 0;
    uint64_t sfmask = 0;
    rflags = 0;

    wrmsr(0xC0000080, rdmsr(0xC0000080) | 1);	//Enable the syscall instruction
    wrmsr(0xC0000081, star_val);
    wrmsr(0xC0000082, lstar);
    wrmsr(0xC0000083, cstar);
    wrmsr(0xC0000084, sfmask);
}