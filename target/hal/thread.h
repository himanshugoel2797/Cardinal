#ifndef _CARDINAL_HAL_THREAD_H_
#define _CARDINAL_HAL_THREAD_H_

#include "types.h"
#include "managers.h"

#define ARCH_SPECIFIC_SPACE_SIZE 16

#define ARCH_DATA_FS_OFFSET 0
#define ARCH_DATA_GS_OFFSET 1

void
SwitchAndInitializeThread(ThreadInfo *thd);

void
SwitchToThread(ThreadInfo *dst);

void
SavePreviousThread(ThreadInfo *src);

void
SetInterruptStack(void* sp);

void
SetFSBase(void *base);

void
SetGSBase(void *base);

void*
GetFSBase(void);

void*
GetGSBase(void);

void
RestoreFPUState(void *dst);

void
SaveFPUState(void *src);

uint64_t
GetFPUStateSize(void);

void
SetupPreemption(void);

void
ResetPreemption(void);

void
SetupArchSpecificData(ThreadInfo *tInfo,
                      CRegisters *regs);

void
PerformArchSpecificTaskSave(ThreadInfo *tInfo);

void
PerformArchSpecificTaskSwitch(ThreadInfo *tInfo);

#endif