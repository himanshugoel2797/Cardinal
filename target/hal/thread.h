#ifndef _CARDINAL_HAL_THREAD_H_
#define _CARDINAL_HAL_THREAD_H_

#include "types.h"
#include "managers.h"

/**
 * \defgroup threading Threading
 * @{
 */

/**
 * The size of the ArchSpecificData area.
 */
extern const uint64_t ARCH_SPECIFIC_SPACE_SIZE;

/**
 * @brief      Switch to and initialize the thread.
 *
 * @param      thd   The thread
 */
void
SwitchAndInitializeThread(ThreadInfo *thd);

/**
 * @brief      Switch to the thread.
 *
 * @param      dst   The thread
 */
void
SwitchToThread(ThreadInfo *dst);

/**
 * @brief      Saves the thread state.
 *
 * @param      src   The source
 */
void
SavePreviousThread(ThreadInfo *src);

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
 * @brief      Sets the kernel gs base.
 *
 * @param      base  The base
 */
void
SetKernelGSBase(void *base);

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
 * @brief      Reset the preemption timer.
 */
void
ResetPreemption(void);

/**
 * @brief      Setup arch specific data.
 *
 * @param      tInfo  The thread information
 * @param      regs   The registers to initialize the thread.
 */
void
SetupArchSpecificData(ThreadInfo *tInfo,
                      CRegisters *regs);

/**
 * @brief      Save arch specific data.
 *
 * @param      tInfo  The t information
 */
void
PerformArchSpecificTaskSave(ThreadInfo *tInfo);

/**
 * @brief      Switch arch specific data.
 *
 * @param      tInfo  The t information
 */
void
PerformArchSpecificTaskSwitch(ThreadInfo *tInfo);

/**@}*/

#endif