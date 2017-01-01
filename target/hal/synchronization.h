/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_HAL_SYNCHRONIZATION_OPS_H_
#define _CARDINAL_HAL_SYNCHRONIZATION_OPS_H_

#include "types.h"

/**
 * \defgroup sync_hal Synchronization primitives.
 * @{
 */

/**
 * Spinlock type.
 */
typedef volatile uint64_t* Spinlock;


void
AutounlockSpinlock(Spinlock *prim);

#define SpinlockUnlocker __attribute__((cleanup(AutounlockSpinlock))) Spinlock

/**
 * @brief      Creates a spinlock.
 *
 * @return     NULL on error, a new spinlock in the unlocked state on success.
 */
Spinlock
CreateSpinlock(void);

/**
 * @brief      Creates a bootstrap spinlock.
 *
 * @return     NULL on error, a new spinlock in the unlocked state on success.
 */
Spinlock
CreateBootstrapSpinlock(void);

/**
 * @brief      Locks the spinlock.
 *
 * @param[in]  primitive  The primitive
 *
 * @return     primitive
 */
Spinlock
LockSpinlock(Spinlock primitive);

/**
 * @brief      Gets the spinlock contender count.
 *
 * @param[in]  primitive  The primitive
 *
 * @return     The spinlock contender count.
 */
uint64_t
GetSpinlockContenderCount(Spinlock primitive);

/**
 * @brief      Unlocks the spinlock.
 *
 * @param[in]  primitive  The primitive
 *
 * @return     TRUE if the lock is still locked, otherwise FALSE.
 */
bool
UnlockSpinlock(Spinlock primitive);

/**
 * @brief      Free a previously allocated spinlock.
 *
 * @param[in]  primitive  The primitive
 */
void
FreeSpinlock(Spinlock primitive);

/**
 * @brief      Atomically increment the specified value.
 *
 * @param      val   The value
 */
void
AtomicIncrement32(uint32_t *val);

/**
 * @brief      Atomically increment the specified value.
 *
 * @param      val   The value
 */
void
AtomicIncrement64(uint64_t *val);

/**
 * @brief      Atomically decrement the specified value.
 *
 * @param      val   The value
 */
void
AtomicDecrement32(uint32_t *val);

/**
 * @brief      Atomically decrement the specified value.
 *
 * @param      val   The value
 */
void
AtomicDecrement64(uint64_t *val);

/**
 * @brief      Atomically decrement the specified value.
 *
 * @param      val   The value
 */
void
AtomicDecrementSize(size_t *val);

/**
 * @brief      Atomically increment the specified value.
 *
 * @param      val   The value
 */
void
AtomicIncrementSize(size_t *val);

/**
 * @brief      Atomically subtract the specified amount from the value.
 *
 * @param      val   The value
 * @param[in]  sub   The sub
 */
void
AtomicSubtractSize(size_t *val, size_t sub);

/**
 * @brief      Atomically add the specified amount to the value.
 *
 * @param      val   The value
 * @param[in]  add   The add
 */
void
AtomicAddSize(size_t *val, size_t add);

/**@}*/

#endif