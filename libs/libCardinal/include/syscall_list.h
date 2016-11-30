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

    Syscall_RequestResponseBuffer,      //!< Request a buffer for responses.
    Syscall_GetResponseKey,             //!< Get a response key for a process to use to respond.
    Syscall_SubmitResponse,             //!< Submit response with a key to send the response to the process.
    Syscall_QueryResponseKeyLength,     //!< Query the length of a response key.

    Syscall_R0_GetPhysicalAddress,      //!< Get a physical address for a virtual address for the provided pid.

    //Syscall_RequestMappingKeyPair,      //!< Request a key pair to authorize other processes to read from and a single process to write to a region of specified length.
    //Syscall_ReturnMappingKey,           //!< Free a mapping key, freeing the associated areas, in the case of a write area key, freeing the read keys as well.
    //Syscall_ActivateMappingKey,         //!< Activate a mapping key.
    //Syscall_DeactivateMappingKey,       //!< Deactivate a mapping key.
    //Syscall_QueryMappingKey,            //!< Get mapping key length and permissions.

    //Syscall_RequestKey,                 //!< Request a key to signal on a specific action.
    //Syscall_WaitForKey,                 //!< Wait for a key to be signalled.
    //Syscall_SignalKey,                  //!< Signal a key.
    //Syscall_ReturnKey,                  //!< Free a key owned by the calling process.

    Syscall_NumEnd = Syscall_R0_GetPhysicalAddress,
} SyscallFunctions;

#ifdef __cplusplus
}
#endif

#endif