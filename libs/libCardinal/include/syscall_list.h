/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
    Syscall_FreeSharedMemoryKey,            //!< Free a shared memory key owned by the calling process.
    
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

    Syscall_WaitForRequest,
    Syscall_HandleRequest,
    Syscall_Signal,                         //!< Send a signal to a process by its PID.

    Syscall_Open,
    Syscall_Read,
    Syscall_Write,
    Syscall_Close,
    Syscall_Remove,
    Syscall_Bind,
    Syscall_GetStat,
    Syscall_ApplyStat,
    
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