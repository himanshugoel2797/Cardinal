// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _LIB_CARDINAL_IPC_H_
#define _LIB_CARDINAL_IPC_H_

#include <stddef.h>
#include <stdint.h>
#include "cardinal_types.h"
#include "keyman.h"
#include "memory.h"
#include "syscall.h"
#include "syscall_list.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup ipc_api IPC API
 * @{
 */

//! The maximum size of a message.
#define MESSAGE_SIZE (256)

//! The process ID of the memory server.
#define MEMORY_SRV_PID 4

//! The process ID of the namespace server.
#define NAMESPACE_SRV_PID 5

//! The process ID of the process server.
#define PROCESS_SRV_PID 3

#ifndef _KERNEL_

/**
 * @brief      Allocate a block of interrupts.
 *
 * @param[in]  cnt   The count
 * @param      irq   The irq number that the block starts from
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t R01AllocateInterrupts(int cnt, int *irq) {
  if (irq == NULL) return -EINVAL;

  *irq = Syscall1(Syscall_R01_AllocateInterrupts, cnt);
  return GetErrno();
}

/**
 * @brief      Register to a previously allocated block of interrupts.
 *
 * @param[in]  irq   The irq
 * @param[in]  cnt   The count
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t R01RegisterInterrupts(int irq, int cnt) {
  Syscall2(Syscall_R01_RegisterForInterrupts, irq, cnt);
  return GetErrno();
}

uint32_t RequestMessageID(void);

bool OpenConnection(UID targetPID);

#endif

/**}@*/

#ifdef __cplusplus
}
#endif

#endif