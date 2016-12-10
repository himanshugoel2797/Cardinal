#ifndef _CARDINAL_INTERRUPT_MAN_H_
#define _CARDINAL_INTERRUPT_MAN_H_

#include "types.h"

void
InterruptMan_Initialize(void);

int
InterruptMan_RegisterProcess(UID pid,
                             int irq,
                             int cnt);

void
InterruptMan_UnregisterProcess(UID pid);

int
InterruptMan_AllocateBlock(int cnt);

#endif