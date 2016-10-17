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

#define SpinlockUnlocker __attribute__((cleanup(AutounlockSpinlock)))

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