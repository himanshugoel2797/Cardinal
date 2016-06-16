#ifndef _CARDINAL_HAL_INTERRUPPTS_H_
#define _CARDINAL_HAL_INTERRUPPTS_H_

#include "types.h"

typedef void(*InterruptHandler)(uint32_t int_no,
                                uint32_t err_code);

uint32_t
RequestInterruptVectorBlock(uint32_t vectorCount);

uint32_t
RegisterInterruptHandler(uint32_t int_no,
                         InterruptHandler handler);

uint32_t
GetInterruptHandler(uint32_t int_no,
                    InterruptHandler *handler);

void
RaiseInterrupt(uint32_t int_no);

void
RemoveInterruptHandler(uint32_t id);

void
SetInterruptEnableMode(uint32_t vector, bool enableMode);

void
HandleInterruptNoReturn(uint32_t vector);

void*
GetSavedInterruptState(void);

#endif
