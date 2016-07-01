#include "sec_mon.h"
#include "filter/filter.h"

#include "libs/libCardinal/include/syscall.h"

static SyscallHandler filtered_syscalls[MAX_SYSCALL_COUNT];

void
SecMon_Initialize(void) {

    for(int i = 0; i < MAX_SYSCALL_COUNT; i++)
        filtered_syscalls[i] = NULL;
}

void
SecMon_RegisterSyscall(uint64_t UNUSED(syscall_num),
                       SyscallHandler UNUSED(handler)) {
}

uint64_t
SecMon_FilterUpdateSyscall(uint64_t UNUSED(ip),
                           uint64_t UNUSED(syscall_num),
                           uint64_t *UNUSED(param)) {
    return 0;
}

void
SecMon_ControlFlowVerificationHandler(uint32_t UNUSED(int_num),
                                      uint32_t UNUSED(err_code)) {

}