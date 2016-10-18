#ifndef _LIBCARDINAL_THREAD_H_
#define _LIBCARDINAL_THREAD_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * \defgroup thread_syscall Threading Syscalls
 * @{
 */

/**
 * @brief      Create a new thread. R0 process only.
 *
 * @param[in]  parent_pid   The parent pid
 * @param[in]  entry_point  The entry point
 * @param      arg          The argument
 * @param      tid          The tid
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline
uint64_t
R0_CreateThread(UID parent_pid,
                int (*entry_point)(void *arg),
                void *arg,
                UID *tid) {
    if(tid != NULL) {
        *tid = Syscall3(Syscall_R0_CreateThread, parent_pid, entry_point, arg);
        return GetErrno();
    }
    return -EINVAL;
}

/**
 * @brief      Set the thread execution status. R0 process only.
 *
 * @param[in]  tid    The tid
 * @param[in]  pause  The pause status
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline
uint64_t
R0_SetThreadIsPaused(UID tid,
                     bool pause) {
    Syscall2(Syscall_R0_SetThreadIsPaused, tid, pause);
    return GetErrno();
}

/**
 * @brief      Kill a thread by its TID. R0 process only.
 *
 * @param[in]  tid   The tid
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline
uint64_t
R0_KillThread(UID tid) {
    Syscall1(Syscall_R0_KillThread, tid);
    return GetErrno();
}

/**
 * @brief      Exit and delete the current thread. R0 process only.
 *
 * @return     Error code on failure, does not return on success.
 */
static __inline
uint64_t
R0_ExitDeleteThread(void) {
    Syscall0(Syscall_R0_ExitDeleteThread);
    return GetErrno();
}

/**@}*/

#endif