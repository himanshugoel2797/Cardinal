#ifndef _CARDINAL_INTERRUPT_MAN_H_
#define _CARDINAL_INTERRUPT_MAN_H_

#include "types.h"

void
InterruptMan_Initialize(void);

void
InterruptMan_RegisterProcess(UID pid, 
							 uint32_t irq, 
							 uint32_t cnt);

void
InterruptMan_UnregisterProcess(UID pid);

#endif