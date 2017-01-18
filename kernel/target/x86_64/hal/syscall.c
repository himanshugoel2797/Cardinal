/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "syscall.h"
#include "utils/native.h"
#include "managers.h"
#include "common/common.h"

typedef struct CoreKernelStackInfo {
    uint8_t *k_stack;
    uint64_t rflags;
} CoreKernelStackInfo;

static volatile CoreKernelStackInfo* k_stack_info;
static SyscallHandler Syscalls[SYSCALL_COUNT];


__attribute__((naked, noreturn))
void
Syscall_Handler(void) {
    //Setup the environment to move into the OS syscall handler
    __asm__ volatile
    (
        "swapgs\n\t"

        //Check if the syscall number in arg 0 is greater than the syscall count
        //If it is, return ENOSYS immediately
        
        //Otherwise, save necessary state and jump to the handler
        "pushq %%rax\n\t"
        "movq %%rsp, %%rax\n\t"
        "movq (k_stack_info), %%rsp\n\t"
        "movq %%r11, +8(%%rsp)\n\t"
        "movq (%%rsp), %%rsp\n\t"
        "pushq %%rax\n\t"
        "pushq %%rcx\n\t"
        "pushq %%rdi\n\t"
        "pushq %%rdx\n\t"
        "pushq %%r8\n\t"
        "pushq %%r9\n\t"
        "pushq %%r10\n\t"
        "pushq %%r11\n\t"


        "xchg %%rcx, %%rdi\n\t"
        "callq (SyscallReceived)\n\t"
        "movq %%rax, %%rsi\n\t"
        "popq %%r11\n\t"
        "popq %%r10\n\t"
        "popq %%r9\n\t"
        "popq %%r8\n\t"
        "popq %%rdx\n\t"
        "popq %%rdi\n\t"
        "popq %%rcx\n\t"
        "popq %%rax\n\t"
        "movq %%rax, %%rsp\n\t"
        "popq %%rax\n\t"
        "movq (k_stack_info), %%r11\n\t"
        "movq +8(%%r11), %%r11\n\t"
        "swapgs\n\t"
        "sysretq\n\t" :::
    );
}

void
SwitchToUserMode(uint64_t pc, uint64_t sp) {
    __asm__ volatile
    (
        "cli\n\t"
        "mov $0, %%r11\n\t"
        "andq $0xFFFFFFFFFFFF0000, %%r11\n\t"
        "orq $512, %%r11\n\t"
        "mov %[sp], %%rsp\n\t"
        "mov %%rsp, %%rbp\n\t"
        "mov $0, %%rdx\n\t"
        //"swapgs\n\t"
        "sysretq\n\t" :: [sp]"r"(sp), "c"(pc)
    );
}

void
SetKernelStack(void* stack) {
    if(k_stack_info == NULL)
        k_stack_info = AllocateAPLSMemory(sizeof(k_stack_info));

    k_stack_info->k_stack = stack;
}

void*
GetKernelStack(void) {
    return k_stack_info->k_stack;
}
uint64_t
GetRFLAGS(void) {
    return k_stack_info->rflags;
}

void
SetRFLAGS(uint64_t val) {
    k_stack_info->rflags = val;
}

void
Syscall_Initialize(void) {
    uint64_t star_val = (0x08ull << 32) | (0x18ull << 48);
    uint64_t lstar = (uint64_t)Syscall_Handler;
    uint64_t sfmask = (uint64_t)-1;

    for(int i = 0; i < SYSCALL_COUNT; i++)
        Syscalls[i] = NULL;

    wrmsr(0xC0000080, rdmsr(0xC0000080) | 1);   //Enable the syscall instruction
    wrmsr(0xC0000081, star_val);
    wrmsr(0xC0000082, lstar);
    wrmsr(0xC0000084, sfmask);
}

void
RegisterSyscall(int syscall_num,
                SyscallHandler handler) {

    if(syscall_num >= SYSCALL_COUNT)
        return;

    if(handler == NULL)
        return;

    Syscalls[syscall_num] = handler;
}