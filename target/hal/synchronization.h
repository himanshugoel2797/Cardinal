#ifndef _CARDINAL_HAL_SYNCHRONIZATION_OPS_H_
#define _CARDINAL_HAL_SYNCHRONIZATION_OPS_H_

#include "types.h"

typedef volatile uint64_t* Spinlock;

Spinlock
CreateSpinlock(void);

Spinlock
CreateBootstrapSpinlock(void);

bool
LockSpinlock(Spinlock primitive);

uint64_t
GetSpinlockContenderCount(Spinlock primitive);

bool
UnlockSpinlock(Spinlock primitive);

void
FreeSpinlock(Spinlock primitive);

void
AtomicIncrement32(uint32_t *val);

void
AtomicIncrement64(uint64_t *val);

void
AtomicDecrement32(uint32_t *val);

void
AtomicDecrement64(uint64_t *val);

void
AtomicDecrementSize(size_t *val);

void
AtomicIncrementSize(size_t *val);

void
AtomicSubtractSize(size_t *val, size_t sub);

void
AtomicAddSize(size_t *val, size_t add);

#endif