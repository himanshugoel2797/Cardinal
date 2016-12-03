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

typedef enum {
    R0_ThreadInfoType_UserStackAddress
} R0_ThreadInfoType;

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
        *tid = Syscall3(Syscall_R0_CreateThread, parent_pid, (uint64_t)entry_point, (uint64_t)arg);
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

/**
 * @brief      Get thread information. R0 process only.
 *
 * @param[in]  tid    The tid
 * @param[in]  type   The type
 * @param      value  The value
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline
uint64_t
R0_GetThreadInfo(UID tid,
                 R0_ThreadInfoType type,
                 uint64_t *value) {
    if(value != NULL) {
        *value = Syscall2(Syscall_R0_GetThreadInfo, tid, type);
        return GetErrno();
    }
    return -EINVAL;
}

/**
 * @brief      Gets the current thread uid.
 *
 * @return     The current thread uid.
 */
static __inline UID
GetCurrentThreadUID(void) {
    UID id = 0;
    GetProperty(CardinalProperty_TID, 0, &id);
    return id;
}

static __inline void*
R0_GetThreadUserStack(UID tid) {
    uint64_t addr = 0;
    R0_GetThreadInfo(tid, R0_ThreadInfoType_UserStackAddress, &addr);
    return addr;
}

static __inline void
SleepThread(uint64_t ns_time) {
    Syscall1(Syscall_Nanosleep, ns_time);
    return GetErrno();
}

static __inline void
WaitForMessage() {
    //TODO design this syscall, use it to reduce the number of active threads by moving polling into the kernel.
}

/**@}*/

#endif