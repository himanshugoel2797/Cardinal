#include "sec_mon.h"
#include "filter/filter.h"

static SyscallHandler filtered_syscalls[MAX_SYSCALL_COUNT];

void
SecMon_Initialize(void) {
    RegisterSyscall(AllocateSyscall(), SecMon_FilterUpdateSyscall);

    for(int i = 0; i < MAX_SYSCALL_COUNT; i++)
        filtered_syscalls[i] = NULL;
}

void
SecMon_RegisterSyscall(uint64_t syscall_num,
                       SyscallHandler handler) {
    filtered_syscalls[syscall_num] = handler;
    RegisterSyscall(syscall_num, UpdateHandler(handler));
}

void
SecMon_FilterUpdateSyscall(uint64_t ip,
                           uint64_t syscall_num,
                           uint64_t *param) {

}

void
SecMon_ControlFlowVerificationHandler(uint32_t int_num,
                                      uint32_t err_code) {

}