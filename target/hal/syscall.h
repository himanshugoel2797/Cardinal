#ifndef _CARDINAL_HAL_SYSCALL_H_
#define _CARDINAL_HAL_SYSCALL_H_

#include "types.h"

/**
 * \defgroup syscall_hal Syscall HAL
 * @{
 */

/**
 * @brief      Initialize the syscall HAL.
 */
void
Syscall_Initialize(void);

/**
 * @brief      Switch to user mode.
 *
 * @param[in]  pc    The location to start execution at
 * @param[in]  sp    The stack pointer value to use
 */
void
SwitchToUserMode(uint64_t pc, uint64_t sp);

/**
 * @brief      Sets the kernel stack.
 *
 * @param      stack  The stack
 */
void
SetKernelStack(void* stack);

/**
 * @brief      Gets the kernel stack.
 *
 * @return     The kernel stack.
 */
void*
GetKernelStack(void);

/**
 * @brief      Setup the security monitor.
 */
void
SetupSecurityMonitor(void);

/**@}*/

#endif