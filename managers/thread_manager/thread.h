/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_THREAD_H_
#define _CARDINAL_THREAD_H_

#include "types.h"
#include "managers/process_manager/process_info.h"
#include "libs/libc/include/signal.h"
#include "libs/libCardinal/include/registers.h"
#include "libs/libCardinal/include/thread.h"
#include "synchronization.h"

/**
 * \addtogroup threading
 * @{
 */

//TODO: For the schedule manager, make sure to delete processes if their state suggests it

/**
 * The default stack size for kernel threads.
 */
#define KERNEL_STACK_SIZE KiB(32)

/**
 * The default stack size for user threads.
 */
#define USER_STACK_SIZE MiB(8)

/**
 * Thread Priority levels.
 */
typedef enum {
    ThreadPriority_VeryLow = 0, //!< Very Low.
    ThreadPriority_Low = 1,     //!< Low.
    ThreadPriority_Medium = 2,  //!< Medium.
    ThreadPriority_Neutral = 3, //!< Neutral.
    ThreadPriority_High = 4,    //!< High.
    ThreadPriority_VeryHigh = 5,//!< Very High.
    ThreadPriority_Maximum = 6  //!< Maximum.
} ThreadPriority;

/**
 * Thread States.
 */
typedef enum {
    ThreadState_Initialize, //!< Thread needs to be initialized.
    ThreadState_Running,    //!< Thread is running.
    ThreadState_Paused,     //!< Thread is paused.
    ThreadState_Sleep,      //!< Thread is asleep.
    ThreadState_Exiting     //!< Thread is exiting.
} ThreadState;

/**
 * Thread Wake Condition.
 */
typedef enum {
    ThreadWakeCondition_None,     //!< No wake condition, thread isn't sleeping.
    ThreadWakeCondition_SleepEnd, //!< Wake on sleep period expiry.
    ThreadWakeCondition_MatchMsgType,
    ThreadWakeCondition_MatchMsgSourcePID,
    ThreadWakeCondition_MatchMsgAny,
} ThreadWakeCondition;

/**
 * Thread permission levels.
 */
typedef enum {
    ThreadPermissionLevel_User,   //!< User level thread.
    ThreadPermissionLevel_Kernel  //!< Kernel level thread.
} ThreadPermissionLevel;

typedef enum {
  ThreadError_None = 0,
  ThreadError_UIDNotFound = 1
} ThreadError;

/**
 * Thread Entry Point.
 */
typedef void (*ThreadEntryPoint)(void*);

/**
 * Thread Information/State.
 */
typedef struct ThreadInfo {

    UID                 ID; //!< Thread ID.

    ProcessInformation  *ParentProcess; //!< Process the thread belongs to.

    ThreadState           State;          //!< Thread state.
    ThreadWakeCondition   WakeCondition;  //!< Thread wake condition.
    ThreadPriority        Priority;       //!< Thread priority.
    ThreadPermissionLevel PermissionLevel;

    uint64_t              InterruptStackBase;     //!< Base of interrupt stack.
    uint64_t              InterruptStackAligned;  //!< Aligned address of interrupt stack.
    uint64_t              KernelStackBase;        //!< Base of kernel stack.
    uint64_t              KernelStackAligned;     //!< Aligned address of kernel stack.
    uint64_t              UserStackBase;          //!< Base of user stack.
    uint64_t              CurrentStack;           //!< Current stack address.

    union {
        uint64_t            SleepDurationNS;        //!< Sleep duration in nanoseconds.
        uint64_t            TargetMsgSourcePID;
        uint64_t            TargetMsgType;
    };

    uint64_t            SleepStartTime;         //!< Sleep start time.
    uint64_t            Errno;                  //!< Errno of last syscall.

    uint64_t            TimeSlice;              //!< The time slice available to this thread.
    int32_t             CoreAffinity;           //!< Core affinity.

    void                *FPUState;              //!< FPU state storage region.
    void                *ArchSpecificData;      //!< Architecture specific data storage region.

    uint32_t            reference_count;        //!< Reference count.
    Spinlock            lock;                   //!< Read/Write lock.
} ThreadInfo;

/**
 * Application Processor information.
 */
typedef struct CoreInfo {
    UID ID;                     //!< Core ID.
    int (*getCoreData)(void);   //!< Get core load data.
} CoreInfo;

/**
 * @brief      Initialize the threading system.
 */
void
Thread_Initialize(void);

/**
 * @brief      Allocate a stack of STACK_SIZE in the specified process.
 *
 * @param[in]  parentProcess  The parent process ID
 * @param[in]  perm_level     The permission level
 *
 * @return     Pointer to the top of the stack.
 */
uint64_t
AllocateStack(UID parentProcess,
              ThreadPermissionLevel perm_level);

/**
 * @brief      Creates a thread.
 *
 * @param[in]  parentProcess  The parent process
 * @param[in]  perm_level     The permission level
 * @param[in]  entry_point    The entry point
 * @param      arg            The argument to the entry point
 *
 * @return     The UID of the new thread.
 */
UID
CreateThread(UID parentProcess,
             ThreadPermissionLevel perm_level,
             ThreadEntryPoint entry_point,
             void *arg);

/**
 * @brief      Creates a thread with just the register state.
 *
 * @param[in]  parentProcess      The parent process
 * @param[in]  user_stack_bottom  The user stack bottom
 * @param      regs               The register state of the process
 *
 * @return     The UID of the new thread.
 */
UID
CreateThreadADV(UID parentProcess,
                ThreadPermissionLevel perm_level,
                uint64_t user_stack_bottom,
                CRegisters *regs);

/**
 * @brief      Sets the thread state.
 *
 * @param[in]  id     The thread ID
 * @param[in]  state  The state
 */
void
SetThreadState(UID id,
               ThreadState state);

/**
 * @brief      Set the thread to sleep for the duration.
 *
 * @param[in]  id           The thread ID
 * @param[in]  duration_ns  The duration in nanoseconds
 */
void
SleepThread(UID id,
            uint64_t duration_ns);

/**
 * @brief      Gets the thread state.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread state.
 */
ThreadState
GetThreadState(UID id);

/**
 * @brief      Gets the thread kernel stack.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread kernel stack.
 */
void*
GetThreadKernelStack(UID id);

/**
 * @brief      Gets the thread current stack.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread current stack.
 */
void*
GetThreadCurrentStack(UID id);

/**
 * @brief      Sets the thread base priority.
 *
 * @param[in]  id        The thread ID
 * @param[in]  priority  The priority
 */
void
SetThreadBasePriority(UID id,
                      ThreadPriority priority);

/**
 * @brief      Gets the thread priority.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread priority.
 */
ThreadPriority
GetThreadPriority(UID id);

/**
 * @brief      Sets the thread core affinity.
 *
 * @param[in]  id      The thread ID
 * @param[in]  coreID  The core id
 */
void
SetThreadCoreAffinity(UID id,
                      int coreID);

/**
 * @brief      Gets the thread core affinity.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread core affinity.
 */
int
GetThreadCoreAffinity(UID id);

/**
 * @brief      Free the thread.
 *
 * @param[in]  id    The thread ID
 */
void
FreeThread(UID id);

/**
 * @brief      Yield the current thread.
 */
void
YieldThread(void);

/**
 * @brief      Sets the periodic preempt vector.
 *
 * @param[in]  irq     The irq
 * @param[in]  period  The period
 */
void
SetPeriodicPreemptVector(uint32_t irq,
                         uint64_t period);

/**
 * @brief      Switch to another thread.
 */
void
SwitchThread(void);

/**
 * @brief      Register a core to the scheduler.
 *
 * @param[in]  id           The core identifier
 * @param[in]  getCoreData  The processor load information provider
 */
void
RegisterCore(int (*getCoreData)(void));

/**
 * @brief      Gets the core load.
 *
 * @param[in]  coreNum  The core number
 *
 * @return     The core load.
 */
int
GetCoreLoad(int coreNum);

/**
 * @brief      Start running the scheduler for the core.
 */
void
CoreUpdate();

/**
 * @brief      Gets the current thread uid.
 *
 * @return     The current thread uid.
 */
UID
GetCurrentThreadUID(void);

/**
 * @brief      Gets the current process uid.
 *
 * @return     The current process uid.
 */
UID
GetCurrentProcessUID(void);

/**
 * @brief      Gets the current process parent uid.
 *
 * @return     The current process parent uid.
 */
UID
GetCurrentProcessParentUID(void);

/**
 * @brief      Gets the current thread information.
 *
 * @return     The current thread information.
 */
ThreadInfo*
GetCurrentThreadInfo(void);

/**
 * @brief      Perform a task switch.
 *
 * @param[in]  int_no    The int no
 * @param[in]  err_code  The error code
 */
void
TaskSwitch(uint32_t int_no,
           uint32_t err_code);

/**
 * @brief      Pause/Resume the specified thread.
 *
 * @param[in]  tid     The thread ID
 * @param[in]  paused  TRUE to pause, FALSE to resume
 */
void
SetThreadIsPaused(UID tid,
                  bool paused);

/**
 * @brief      Sets the thread errno.
 *
 * @param[in]  tid    The tid
 * @param[in]  errno  The errno
 */
void
SetThreadErrno(UID tid,
               uint64_t errno);

/**
 * @brief      Gets the thread errno.
 *
 * @param[in]  tid   The tid
 *
 * @return     The thread errno.
 */
uint64_t
GetThreadErrno(UID tid);

/**
 * @brief      Sleep a thread for a message.
 *
 * @param[in]  id         The identifier
 * @param[in]  wait_type  The wait type
 * @param[in]  val        The value
 */
void
SleepThreadForMessage(UID id,
                      MessageWaitType wait_type,
                      uint64_t val);

/**
 * @brief      Wake the thread.
 *
 * @param[in]  id    The identifier
 */
void
WakeThread(UID id);

/**
 * @brief      Wake threads that are ready to be woken.
 */
void
WakeReadyThreads(void);


/**@}*/

#endif