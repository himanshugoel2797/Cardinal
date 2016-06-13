#ifndef _CARDINAL_HAL_THREAD_H_
#define _CARDINAL_HAL_THREAD_H_

#include "managers.h"

void
SwitchAndInitializeThread(ThreadInfo *thd);

void
SwapThreadOnInterrupt(ThreadInfo *src,
                      ThreadInfo *dst);

void
SetKernelStack(void* sp);

void
SetUserStack(void *sp);

void
SetTLSBase(void *tlsBase);

void
RestoreFPUState(void *dst);

void
SaveFPUState(void *src);

uint64_t
GetFPUStateSize(void);

#endif