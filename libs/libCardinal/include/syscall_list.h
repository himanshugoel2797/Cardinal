#ifndef _LIB_CARDINAL_SYSCALL_LIST
#define _LIB_CARDINAL_SYSCALL_LIST

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Syscall_NumStart = 1,
    Syscall_Brk = Syscall_NumStart,
    Syscall_Nanosleep,

    Syscall_GetIPCMessageFrom,
    Syscall_PostIPCMessage,

    Syscall_SetProperty,
    Syscall_GetProperty,

    Syscall_GetErrno,                   //!< Get the error for the latest syscall

    Syscall_R0_Map,                     //!< Kernel level memory map that allows direct access to the kernel arbitrary memory mapping functions.
    Syscall_R0_Unmap,                   //!< Unmap a mapping.
    Syscall_R0_AllocatePages,           //!< Allocate a continuous range of pages.
    Syscall_R0_FreePages,               //!< Free a continuous range of pages.
    Syscall_R0_CreateProcess,           //!< Create a process.
    Syscall_R0_StartProcess,            //!< Start a process.
    Syscall_R0_GetProcessInfo,          //!< Get the kernel's process information.
    Syscall_R0_SetProcessInfo,          //!< Set the kernel's process information.
    Syscall_R0_GetThreadInfo,           //!< Get the kernel's thread information.
    Syscall_R0_CreateThread,            //!< Create a new thread.
    Syscall_R0_WipeMemoryMap,           //!< Wipe the memory map of a process.
    Syscall_R0_SetThreadIsPaused,       //!< Allow pausing/resuming threads.
    Syscall_R0_KillThread,              //!< Kill a thread by its TID.
    Syscall_R0_ExitDeleteThread,        //!< Exit and delete the current thread.
    Syscall_R0_KillProcess,             //!< Kill a process by its PID.
    Syscall_R0_GetBootInfo,             //!< Get the boot information.

    Syscall_R0_GetPhysicalAddress,      //!< Get a physical address for a virtual address for the provided pid.

    Syscall_Unmap,
    Syscall_AllocateSharedMemory,
    Syscall_GetSharedMemoryKey,
    Syscall_ApplySharedMemoryKey,
    Syscall_FreeSharedMemoryKey,
    Syscall_R0_AllocateSharedMemory,

    Syscall_NumEnd = Syscall_R0_AllocateSharedMemory,
} SyscallFunctions;

#ifdef __cplusplus
}
#endif

#endif