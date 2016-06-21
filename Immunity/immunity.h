#ifndef _IMMUNITY_MAIN_H_
#define _IMMUNITY_MAIN_H_

#include "types.h"
#include "managers.h"

void
Immunity_Initialize(void);

void
Immunity_RegisterSyscall(uint64_t syscall_num,
                         SyscallHandler handler);

void
Immunity_FilterUpdateSyscall(uint64_t ip,
                             uint64_t syscall_num,
                             uint64_t *param);

void
Immunity_ControlFlowVerificationHandler(uint32_t int_num,
                                        uint32_t err_code);


#endif