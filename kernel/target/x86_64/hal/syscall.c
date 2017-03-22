/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "syscall.h"
#include "utils/native.h"
#include "managers.h"
#include "common/common.h"

typedef struct CoreKernelStackInfo {
    uint8_t* k_stack;
    uint64_t rflags;
} CoreKernelStackInfo;

static volatile CoreKernelStackInfo* k_stack_info;
static SyscallHandler Syscalls[SYSCALL_COUNT];

__attribute__((naked, noreturn)) void Syscall_Handler(void) {
    // Setup the environment to move into the OS syscall handler
    __asm__ volatile(
        "swapgs\n\t"

        // Check if the syscall number in arg 0 is greater than the syscall count
        // If it is, return immediately, return value is unchanged.

        "cmp $"S_(SYSCALL_COUNT)", %%r12\n\t"
        "jge exit_syscall_handler\n\t"

        // Otherwise, save necessary state and jump to the handler
        "pushq %%rax\n\t"             //Save rax to make it available for use
        "movq %%rsp, %%rax\n\t"       //Backup the stack pointer
        "movq (k_stack_info), %%rsp\n\t"  //Switch to the new tack
        "movq %%r11, +8(%%rsp)\n\t"   //Save the RFLAGS register
        "movq (%%rsp), %%rsp\n\t"     //Finish the stack switch
        "pushq %%rax\n\t"             //Save the old stack pointer
        "pushq %%rcx\n\t"             //Save SYSCALL instruction data
        "pushq %%rdi\n\t"             //Save scratch registers
        "pushq %%rdx\n\t"
        "pushq %%r8\n\t"
        "pushq %%r9\n\t"
        "pushq %%r10\n\t"
        "pushq %%r11\n\t"
        "pushq %%r14\n\t"
        "xchgq %%r14, %%rcx\n\t"    //Swap rcx and r14 for the parameters

        "movq $Syscalls, %%rax\n\t"
        "movq (%%rax, %%r12, 8), %%rax\n\t"  //Call Syscalls[%r12]
        "callq *%%rax\n\t"

        "movq %%rax, %%r13\n\t"       //Store the return value in r13

        "popq %%r14\n\t"
        "popq %%r11\n\t"              //Restore the scratch registers
        "popq %%r10\n\t"
        "popq %%r9\n\t"
        "popq %%r8\n\t"
        "popq %%rdx\n\t"
        "popq %%rdi\n\t"
        "popq %%rcx\n\t"
        "popq %%rax\n\t"
        "movq %%rax, %%rsp\n\t"       //Restore the old stack pointer
        "popq %%rax\n\t"              //Restore rax
        "movq (k_stack_info), %%r11\n\t"  //Restore RFLAGS
        "movq +8(%%r11), %%r11\n\t"


        "exit_syscall_handler:\n\t"
        "swapgs\n\t"
        "sysretq\n\t" ::
        :);
}

void SwitchToUserMode(uint64_t pc, uint64_t sp) {
    __asm__ volatile(
        "cli\n\t"
        "mov $0, %%r11\n\t"
        "andq $0xFFFFFFFFFFFF0000, %%r11\n\t"
        "orq $512, %%r11\n\t"
        "mov %[sp], %%rsp\n\t"
        "mov %%rsp, %%rbp\n\t"
        "mov $0, %%rdx\n\t"
        //"swapgs\n\t"
        "sysretq\n\t" ::[sp] "r"(sp),
        "c"(pc));
}

void SetKernelStack(void* stack) {
    if (k_stack_info == NULL)
        k_stack_info = AllocateAPLSMemory(sizeof(k_stack_info));

    k_stack_info->k_stack = stack;
}

void* GetKernelStack(void) {
    return k_stack_info->k_stack;
}
uint64_t GetRFLAGS(void) {
    return k_stack_info->rflags;
}

void SetRFLAGS(uint64_t val) {
    k_stack_info->rflags = val;
}

void Syscall_Initialize(void) {
    uint64_t star_val = (0x08ull << 32) | (0x18ull << 48);
    uint64_t lstar = (uint64_t)Syscall_Handler;
    uint64_t sfmask = (uint64_t)-1;

    wrmsr(0xC0000080, rdmsr(0xC0000080) | 1);  // Enable the syscall instruction
    wrmsr(0xC0000081, star_val);
    wrmsr(0xC0000082, lstar);
    wrmsr(0xC0000084, sfmask);
}

void RegisterSyscall(int syscall_num, SyscallHandler handler) {
    if (syscall_num >= SYSCALL_COUNT) return;

    if (handler == NULL) return;

    Syscalls[syscall_num] = handler;
}