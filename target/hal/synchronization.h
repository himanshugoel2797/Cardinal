#ifndef _CARDINAL_HAL_SYNCHRONIZATION_OPS_H_
#define _CARDINAL_HAL_SYNCHRONIZATION_OPS_H_

#include "types.h"

typedef uint64_t* Spinlock;

Spinlock
CreateSpinlock(void);

void
LockSpinlock(Spinlock *primitive);

void
UnlockSpinlock(Spinlock *primitive);

void
FreeSpinlock(Spinlock *primitive);

#endif