#ifndef _CARDINAL_X86_64_SYSCALL_H_
#define _CARDINAL_X86_64_SYSCALL_H_

#include "types.h"

uint64_t
GetRFLAGS(void);

void
SetRFLAGS(uint64_t val);

uint64_t
ArchPrctl_Syscall(uint64_t target,
                  uint64_t val);

uint64_t
SecurityMonitor_IOPL(uint64_t val);

#endif