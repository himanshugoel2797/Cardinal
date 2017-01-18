/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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