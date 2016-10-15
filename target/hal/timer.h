#ifndef _CARDINAL_HAL_TIMER_H_
#define _CARDINAL_HAL_TIMER_H_

#include "types.h"

/**
 * \defgroup timer_hal Timer
 * @{
 */

/**
 * @brief      Initialize the timer.
 */
void
InitializeTimer(void);

/**
 * @brief      Gets the timer value.
 *
 * @return     The timer value.
 */
uint64_t
GetTimerValue(void);

/**
 * @brief      Sets the timer value.
 *
 * @param[in]  val   The value
 */
void
SetTimerValue(uint64_t val);

/**
 * @brief      Sets the timer enable mode.
 *
 * @param[in]  enabled  TRUE if the timer is enabled, FALSE if the timer is disabled
 */
void
SetTimerEnableMode(bool enabled);

/**
 * @brief      Gets the timer interval in ns.
 *
 * @param[in]  diff  The difference between two reads
 *
 * @return     The timer interval in ns.
 */
uint64_t
GetTimerInterval_NS(uint64_t diff);

/**@}*/

#endif