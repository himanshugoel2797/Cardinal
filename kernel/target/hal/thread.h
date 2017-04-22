// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

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