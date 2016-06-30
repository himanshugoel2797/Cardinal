#ifndef _CARDINAL_HAL_THREAD_H_
#define _CARDINAL_HAL_THREAD_H_

#include "managers.h"

void
SwitchAndInitializeThread(ThreadInfo *thd);

void
SwitchToThread(ThreadInfo *dst);

void
SavePreviousThread(ThreadInfo *src);

void
SetInterruptStack(void* sp);

void
SetTLSBase(void *tlsBase);

void
RestoreFPUState(void *dst);

void
SaveFPUState(void *src);

uint64_t
GetFPUStateSize(void);

void
ApplyProcessPermissions(ProcessPermissions perms);

#endif