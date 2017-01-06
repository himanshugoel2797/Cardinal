/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_HAL_THREAD_H_
#define _CARDINAL_HAL_THREAD_H_

#include "types.h"
#include "managers.h"

#include "arch_defs.h"

/**
 * \defgroup threading Threading
 * @{
 */

/**
 * The size of the ArchSpecificData area.
 */
extern const uint64_t ARCH_SPECIFIC_SPACE_SIZE;

/**
 * @brief      Sets the interrupt stack.
 *
 * @param      sp    The interrupt stack
 */
void
SetInterruptStack(void* sp);

/**
 * @brief      Sets the fs base.
 *
 * @param      base  The base
 */
void
SetFSBase(void *base);

/**
 * @brief      Sets the gs base.
 *
 * @param      base  The base
 */
void
SetGSBase(void *base);

/**
 * @brief      Gets the fs base.
 *
 * @return     The fs base.
 */
void*
GetFSBase(void);

/**
 * @brief      Sets the background gs base.
 *
 * @param      base  The base
 */
void
SetBG_GSBase(void *base);

/**
 * @brief      Gets the background gs base.
 *
 * @return     The background gs base.
 */
void*
GetBG_GSBase(void);

/**
 * @brief      Gets the gs base.
 *
 * @return     The gs base.
 */
void*
GetGSBase(void);

/**
 * @brief      Gets the gs.
 *
 * @return     The gs.
 */
void*
GetGS(void);

/**
 * @brief      Restore the FPU state from the provided data.
 *
 * @param[in]  dst   The data
 */
void
RestoreFPUState(void *dst);

/**
 * @brief      Saves the fpu state to src.
 *
 * @param      src   The source
 */
void
SaveFPUState(void *src);

/**
 * @brief      Gets the fpu state size.
 *
 * @return     The fpu state size.
 */
uint64_t
GetFPUStateSize(void);

/**
 * @brief      Setup preemption.
 */
void
SetupPreemption(void);

/**
 * @brief      Configure preemption.
 *
 * @param[in]  timeSlice  The time slice
 */
void
ConfigurePreemption(int64_t timeSlice);

/**
 * @brief      Adds some preemtion time to the currently executing thread.
 *
 * @param[in]  amnt  The amnt
 */
void
AddPreemtionTime(int64_t amnt);

/**
 * @brief      Setup arch specific data.
 *
 * @param      tInfo  The thread information
 * @param      regs   The registers to initialize the thread.
 */
void
SetupArchSpecificData(ThreadInfo *tInfo,
                      Registers *regs,
                      void *tls);

/**
 * @brief      Save the current task to the specified ThreadInfo struct from the given Registers struct.
 *
 * @param      tInfo  The thread information
 */
void
SaveTask(ThreadInfo *tInfo,
		 Registers *regs);

/**
 * @brief      Switch tasks.
 *
 * @param      tInfo  The thread information
 */
void
SwitchToTask(ThreadInfo *tInfo);

/**@}*/

#endif