#ifndef _CARDINAL_SECURITY_MONITOR_MAIN_H_
#define _CARDINAL_SECURITY_MONITOR_MAIN_H_

#include "types.h"
#include "managers.h"

void
SecMon_Initialize(void);

void
SecMon_RegisterSyscall(uint64_t syscall_num,
                       SyscallHandler handler);

uint64_t
SecMon_FilterUpdateSyscall(uint64_t ip,
                           uint64_t syscall_num,
                           uint64_t *param);

void
SecMon_ControlFlowVerificationHandler(uint32_t int_num,
                                      uint32_t err_code);


#endif