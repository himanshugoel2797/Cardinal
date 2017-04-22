// Copyright (c) 2017 Himanshu Goel
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _LIB_CARDINAL_SYSCALL_LIST
#define _LIB_CARDINAL_SYSCALL_LIST

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum {
    Syscall_NumStart = 1,

    Syscall_Nanosleep = Syscall_NumStart,
    Syscall_R0_GetBootInfo,                 //!< Get the boot information.

    Syscall_SetProperty,
    Syscall_GetProperty,

    Syscall_GetErrno,                       //!< Get the error for the latest syscall

    Syscall_AllocateSharedMemory,           //!< Allocate shared memory.
    Syscall_R0_AllocateSharedMemory,        //!< Allocate shared memory, controlling the physical address.
    Syscall_ApplySharedMemoryKey,           //!< Apply an existing shared memory key.
    Syscall_GetSharedMemoryKey,             //!< Get a shared memory key.

    Syscall_R0_Map,                         //!< Kernel level memory map that allows direct access to the kernel arbitrary memory mapping functions.
    Syscall_R0_Unmap,                       //!< Unmap a mapping.
    Syscall_Unmap,                          //!< Unmap any shared memory region.
    Syscall_Brk,

    Syscall_R0_AllocatePages,               //!< Allocate a continuous range of pages.
    Syscall_R0_FreePages,                   //!< Free a continuous range of pages.
    Syscall_R01_GetPhysicalAddress,         //!< Get a physical address for a virtual address for the provided pid.

    Syscall_CreateProcess,                  //!< Load an executable from memory, along with descriptor copies.

    Syscall_R0_GetThreadInfo,               //!< Get the kernel's thread information.
    Syscall_CreateThread,                   //!< Create a new thread.
    Syscall_KillThread,                     //!< Kill a thread by its TID.
    Syscall_ExitDeleteThread,               //!< Exit and delete the current thread.
    Syscall_SetThreadIsPaused,              //!< Allow pausing/resuming threads.

    Syscall_R01_AllocateInterrupts,         //!< Allocate a block of interrupts
    Syscall_R01_RegisterForInterrupts,      //!< Register for a block of interrupts

    Syscall_WaitSignal,
    Syscall_HandleSignal,
    Syscall_Signal,                         //!< Send a signal to a process by its name.

    Syscall_RegisterName,                   //!< Allow applications to register paths as identifiers, giving options to configure how to handle overlapping names and ownership.
    Syscall_UnregisterName,

    Syscall_CreateKey,
    Syscall_AddKey,
    Syscall_RemoveKey,
    Syscall_HasKey,
    Syscall_UseKey,
    Syscall_GetKeyUsageCount,               //!< Get the number of times a key has been used.

    Syscall_NumEnd = Syscall_GetKeyUsageCount,
} SyscallFunctions;

#ifdef __cplusplus
}
#endif

#endif