#include "syscall.h"

void
SyscallReceived(uint64_t instruction_pointer,
                uint64_t syscall_num,
                uint64_t *syscall_params) {
    instruction_pointer = 0;
    syscall_num = 0;
    syscall_params = NULL;
    while(1) __asm__ ("cli\n\thlt" :: "a"(syscall_num));
}