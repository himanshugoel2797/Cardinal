// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _CARDINAL_THREAD_H_
#define _CARDINAL_THREAD_H_

#include "libs/libCardinal/include/thread.h"
#include "memory.h"
#include "synchronization.h"
#include "types.h"

#include "arch_defs.h"

/**
 * \addtogroup threading
 * @{
 */

/**
 * The default stack size for kernel threads.
 */
#define KERNEL_STACK_SIZE KiB(32)

/**
 * The default stack size for user threads.
 */
#define USER_STACK_SIZE MiB(8)

#define THREAD_TOTAL_TIMESLICE 0xFFFF

#define THREAD_PREEMPTION_COST 0x0FFF

#define THREAD_IPC_COST 0x000F

#define THREAD_INIT_TIMESLICE THREAD_PREEMPTION_COST

#define MAX_THREADS_PER_SWAP 0x10

#define ROOT_PID 1

/**
 * Thread States.
 */
typedef enum {
    ThreadState_Created,     //!< Thread has been created but not set as ready.
    ThreadState_Initialize,  //!< Thread needs to be initialized.
    ThreadState_Running,     //!< Thread is running.
    ThreadState_Paused,      //!< Thread is paused.
    ThreadState_Sleep,       //!< Thread is asleep.
    ThreadState_Exiting      //!< Thread is exiting.
} ThreadState;

/**
 * Thread Wake Condition.
 */
typedef enum {
    ThreadWakeCondition_None,      //!< No wake condition, thread isn't sleeping.
    ThreadWakeCondition_SleepEnd,  //!< Wake on sleep period expiry.
    ThreadWakeCondition_Signal,
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
    ThreadError_UIDNotFound = 1,
    ThreadError_InvalidParams,
    ThreadError_Deleting,
    ThreadError_Unknown,
} ThreadError;

typedef enum {
    ProcessStatus_Executing,
    ProcessStatus_Terminating,
} ProcessStatus;

typedef struct {
    uint64_t desc;
} DescriptorTable;

/**
 * Thread Entry Point.
 */
typedef void (*ThreadEntryPoint)(void *);

typedef struct ProcessInfo {
    Spinlock lock;

    UID PID;
    UID ParentID;
    UID UserID;
    UID GroupID;

    ProcessStatus Status;

    uint64_t HeapBreak;

    // TODO: Use descriptor system for tracking interrupt registration
    DescriptorTable Descriptors;

    ManagedPageTable *PageTable;
} ProcessInfo;

/**
 * Thread Information/State.
 */
typedef struct ThreadInfo {
    Spinlock lock;  //!< Read/Write lock.

    UID ID;  //!< Thread ID.

    ProcessInfo *Process;

    ThreadState State;                  //!< Thread state.
    ThreadWakeCondition WakeCondition;  //!< Thread wake condition.
    ThreadPermissionLevel PermissionLevel;

    uint64_t KernelStackBase;     //!< Base of kernel stack.
    uint64_t KernelStackAligned;  //!< Aligned address of kernel stack.
    uint64_t UserStackBase;       //!< Base of user stack.
    uint64_t CurrentStack;        //!< Current stack address.

    uint64_t SleepDurationNS;  //!< Sleep duration in nanoseconds.
    uint64_t SleepStartTime;   //!< Sleep start time.
    uint64_t Errno;            //!< Errno of last syscall.

    uint64_t CoreAffinity;
    int64_t TimeSlice;  //!< The time slice available to this thread.

    void *FPUState;          //!< FPU state storage region.
    void *ArchSpecificData;  //!< Architecture specific data storage region.

} ThreadInfo;

/**
 * Application Processor information.
 */
typedef struct CoreInfo {
    UID ID;                    //!< Core ID.
    int (*getCoreData)(void);  //!< Get core load data.
} CoreInfo;

/**
 * @brief      Initialize the threading system.
 */
void Thread_Initialize(void);

bool Thread_IsInitialized(void);

/**
 * @brief      Allocate a stack of STACK_SIZE in the specified process.
 *
 * @param[in]  parentProcess  The parent process ID
 * @param[in]  perm_level     The permission level
 *
 * @return     Pointer to the top of the stack.
 */
uint64_t AllocateStack(ProcessInfo *pInfo, ThreadPermissionLevel perm_level);

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
UID CreateThread(UID parentProcess, bool newProcess,
                 ThreadPermissionLevel perm_level, ThreadEntryPoint entry_point,
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
UID CreateThreadADV(ProcessInfo *pInfo, bool newProcess,
                    ThreadPermissionLevel perm_level,
                    uint64_t user_stack_bottom, Registers *regs, void *tls);

/**
 * @brief      Sets the thread state.
 *
 * @param[in]  id     The thread ID
 * @param[in]  state  The state
 */
void SetThreadState(UID id, ThreadState state);

/**
 * @brief      Set the thread to sleep for the duration.
 *
 * @param[in]  id           The thread ID
 * @param[in]  duration_ns  The duration in nanoseconds
 */
void SleepThread(UID id, uint64_t duration_ns);

/**
 * @brief      Gets the thread state.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread state.
 */
ThreadState GetThreadState(UID id);

/**
 * @brief      Gets the thread kernel stack.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread kernel stack.
 */
void *GetThreadKernelStack(UID id);

/**
 * @brief      Gets the thread current stack.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread current stack.
 */
void *GetThreadCurrentStack(UID id);

/**
 * @brief      Sets the thread core affinity.
 *
 * @param[in]  id      The thread ID
 * @param[in]  coreID  The core id
 */
void SetThreadCoreAffinity(UID id, int coreID);

/**
 * @brief      Gets the thread core affinity.
 *
 * @param[in]  id    The thread ID
 *
 * @return     The thread core affinity.
 */
int GetThreadCoreAffinity(UID id);

/**
 * @brief      Free the thread.
 *
 * @param[in]  id    The thread ID
 */
void FreeThread(UID id);

/**
 * @brief      Yield the current thread.
 */
uint64_t YieldThread(void);

/**
 * @brief      Sets the periodic preempt vector.
 *
 * @param[in]  irq     The irq
 * @param[in]  period  The period
 */
void SetPeriodicPreemptVector(uint32_t irq);

/**
 * @brief      Switch to another thread.
 */
void SwitchThread(void);

/**
 * @brief      Register a core to the scheduler.
 *
 * @param[in]  id           The core identifier
 * @param[in]  getCoreData  The processor load information provider
 */
void RegisterCore(int (*getCoreData)(void));

/**
 * @brief      Gets the core load.
 *
 * @param[in]  coreNum  The core number
 *
 * @return     The core load.
 */
int GetCoreLoad(int coreNum);

/**
 * @brief      Start running the scheduler for the core.
 */
void CoreUpdate();

/**
 * @brief      Gets the current thread uid.
 *
 * @return     The current thread uid.
 */
UID GetCurrentThreadUID(void);

/**
 * @brief      Gets the current process uid.
 *
 * @return     The current process uid.
 */
UID GetCurrentProcessUID(void);

/**
 * @brief       Gets the process group id.
 *
 * @param[in]   pid     The Process ID
 *
 * @return      The Group ID.
 */
UID GetProcessGroupID(UID pid);

/**
 * @brief       Sets the process group id.
 *
 * @param[in]   pid     The Process ID
 * @param[in]   gid     The Group ID
 *
 * @return      -1 on error, Group ID otherwise.
 */
uint64_t SetProcessGroupID(UID pid, UID gid);

/**
 * @brief      Gets the current process parent uid.
 *
 * @return     The current process parent uid.
 */
UID GetCurrentProcessParentUID(void);

/**
 * @brief      Gets the current thread information.
 *
 * @return     The current thread information.
 */
ThreadInfo *GetCurrentThreadInfo(void);

ThreadError GetProcessReference(UID pid, ProcessInfo **pInfo);

ThreadError ReturnProcessReference(UID pid);

void SetCoreAffinity(UID tid, uint64_t affinity);

/**
 * @brief      Perform a task switch.
 *
 * @param[in]  int_no    The int no
 * @param[in]  err_code  The error code
 */
void TaskSwitch(uint32_t int_no, uint32_t err_code);

/**
 * @brief      Pause/Resume the specified thread.
 *
 * @param[in]  tid     The thread ID
 * @param[in]  paused  TRUE to pause, FALSE to resume
 */
void SetThreadIsPaused(UID tid, bool paused);

/**
 * @brief      Sets the thread errno.
 *
 * @param[in]  tid    The tid
 * @param[in]  errno  The errno
 */
void SetThreadErrno(UID tid, uint64_t errno);

/**
 * @brief      Gets the thread errno.
 *
 * @param[in]  tid   The tid
 *
 * @return     The thread errno.
 */
uint64_t GetThreadErrno(UID tid);

/**
 * @brief      Sleep a thread for a message.
 *
 * @param[in]  id         The identifier
 * @param[in]  wait_type  The wait type
 * @param[in]  val        The value
 */
void SleepThreadForMessage(UID id, MessageWaitType wait_type, uint64_t val);

/**
 * @brief      Wake the thread.
 *
 * @param[in]  id    The identifier
 */
void WakeThread(UID id);

/**
 * @brief      Wake threads that are ready to be woken.
 */
void WakeReadyThreads(void);

/**
 * @brief      Adds to a thread's time slice.
 *
 * @param[in]  tid    The tid
 * @param[in]  slice  The amount to add to the slice
 *
 * @return     The new value of the time slice.
 */
int64_t AddThreadTimeSlice(UID tid, int64_t slice);

/**
 * @brief      Gets the core index.
 *
 * @return     The core index.
 */
uint64_t GetCoreIndex(void);

/**
 * @brief      Gets the active core count.
 *
 * @return     The active core count.
 */
uint64_t GetActiveCoreCount(void);

/**
 * @brief      Perform a single cycle of the scheduler.
 *
 * @param      regs  The regs
 */
void SchedulerCycle(Registers *regs);

/**
 * @brief      Gets the thread reference.
 *
 * @param[in]  id    The identifier
 * @param      thd   The ThreadInfo structure
 *
 * @return     ThreadError_None on success, Error code on failure.
 */
ThreadError GetThreadReference(UID id, ThreadInfo **thd);

ThreadError AddKey(UID pid, Key_t *key);

ThreadError HasKey(UID pid, Key_t *key, bool *hasKey);

ThreadError RemoveKey(UID pid, Key_t *key);

ThreadError GetKey(UID pid, uint32_t idx, Key_t *key);

/**@}*/

#endif