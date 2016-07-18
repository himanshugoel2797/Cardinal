#ifndef _CARDINAL_HAL_SYSCALL_H_
#define _CARDINAL_HAL_SYSCALL_H_

#include "types.h"

void
Syscall_Initialize(void);

void
SwitchToUserMode(uint64_t pc, uint64_t sp);

void
SetKernelStack(void* stack);

void*
GetKernelStack(void);

void
SetupSecurityMonitor(void);

#endif