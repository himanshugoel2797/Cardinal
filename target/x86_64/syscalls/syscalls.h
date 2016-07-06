#ifndef _CARDINAL_X86_64_SYSCALL_H_
#define _CARDINAL_X86_64_SYSCALL_H_

#include "types.h"

uint64_t
GetRFLAGS(void);

void
SetRFLAGS(uint64_t val);

uint64_t
SecurityMonitor_SyscallReceived(uint64_t instruction_pointer,
                                uint64_t syscall_num,
                                uint64_t *syscall_params);

uint64_t
SecurityMonitor_IOPL(uint64_t UNUSED(instruction_pointer),
                     uint64_t syscall_num,
                     uint64_t *syscall_params);

#endif