/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LIBCARDINAL_THREAD_H_
#define _LIBCARDINAL_THREAD_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * \defgroup thread_syscall Threading Syscalls
 * @{
 */

typedef enum { R0_ThreadInfoType_UserStackAddress } R0_ThreadInfoType;

typedef enum {
    MessageWaitType_Any,
    MessageWaitType_MsgType,
    MessageWaitType_SourcePID
} MessageWaitType;

#ifndef _KERNEL_

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
static __inline uint64_t CreateThread(int (*entry_point)(void *arg), void *arg,
                                      UID *tid) {
    if (tid != NULL) {
        *tid = Syscall2(Syscall_CreateThread, (uint64_t)entry_point, (uint64_t)arg);
        return GetErrno();
    }
    return -EINVAL;
}

/**
 * @brief      Set the thread execution status.
 *
 * @param[in]  tid    The tid
 * @param[in]  pause  The pause status
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t SetThreadIsPaused(UID tid, bool pause) {
    Syscall2(Syscall_SetThreadIsPaused, tid, pause);
    return GetErrno();
}

/**
 * @brief      Kill a thread by its TID.
 *
 * @param[in]  tid   The tid
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t KillThread(UID tid) {
    Syscall1(Syscall_KillThread, tid);
    return GetErrno();
}

/**
 * @brief      Exit and delete the current thread.
 *
 * @return     Error code on failure, does not return on success.
 */
static __inline uint64_t ExitDeleteThread(void) {
    Syscall0(Syscall_ExitDeleteThread);
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
static __inline uint64_t R0_GetThreadInfo(UID tid, R0_ThreadInfoType type,
        uint64_t *value) {
    if (value != NULL) {
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
static __inline UID GetCurrentThreadUID(void) {
    UID id = 0;
    GetProperty(CardinalProperty_TID, 0, &id);
    return id;
}

static __inline void *R0_GetThreadUserStack(UID tid) {
    uint64_t addr = 0;
    R0_GetThreadInfo(tid, R0_ThreadInfoType_UserStackAddress, &addr);
    return (void *)addr;
}

static __inline void SleepThread(uint64_t ns_time) {
    Syscall1(Syscall_Nanosleep, ns_time);
}

#endif

/**@}*/

#endif