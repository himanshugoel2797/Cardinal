/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_HAL_INTERRUPPTS_H_
#define _CARDINAL_HAL_INTERRUPPTS_H_

#include "types.h"

/**
 * \defgroup interrupt_man Interrupt Management
 * @{
 */

/**
 * An interrupt handler.
 */
typedef void(*InterruptHandler)(uint32_t int_no,
                                uint32_t err_code);

/**
 * @brief      Request a continuous block of interrupt vectors.
 *
 * @param[in]  vectorCount  The vector count
 *
 * @return     The base interrupt number from which the block starts.
 */
uint32_t
RequestInterruptVectorBlock(uint32_t vectorCount);

/**
 * @brief      Register an interrupt handler.
 *
 * @param[in]  int_no   The interrupt number
 * @param[in]  handler  The handler
 *
 * @return     0 on success, -1 on error.
 */
uint32_t
RegisterInterruptHandler(uint32_t int_no,
                         InterruptHandler handler);

/**
 * @brief      Gets the interrupt handler.
 *
 * @param[in]  int_no   The int no
 * @param[out] handler  The handler
 *
 * @return     The interrupt handler.
 */
uint32_t
GetInterruptHandler(uint32_t int_no,
                    InterruptHandler *handler);

/**
 * @brief      Raise the specified interrupt number.
 *
 * @param[in]  int_no  The interrupt number
 */
void
RaiseInterrupt(uint32_t int_no);

/**
 * @brief      Removes an interrupt handler.
 *
 * @param[in]  id    The interrupt number for which to remove the handler
 */
void
RemoveInterruptHandler(uint32_t id);

/**
 * @brief      Sets the interrupt enable mode.
 *
 * @param[in]  vector      The interrupt number
 * @param[in]  enableMode  The enable mode
 */
void
SetInterruptEnableMode(uint32_t vector, bool enableMode);

/**
 * @brief      Handle the interrupt with the assumption that execution will not return.
 *
 * @param[in]  vector  The vector
 */
void
HandleInterruptNoReturn(uint32_t vector);

/**
 * @brief      Gets the saved interrupt state.
 *
 * @return     The saved interrupt state.
 */
void*
GetSavedInterruptState(void);

/**@}*/

#endif
