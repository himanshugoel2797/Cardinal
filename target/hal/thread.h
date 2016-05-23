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
TaskSwitch(uint32_t int_no,
           uint32_t err_code);

#endif