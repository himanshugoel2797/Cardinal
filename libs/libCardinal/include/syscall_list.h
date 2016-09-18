#ifndef _LIB_CARDINAL_SYSCALL_LIST
#define _LIB_CARDINAL_SYSCALL_LIST

typedef enum {
    Syscall_NumStart = 1,
    Syscall_MMap = Syscall_NumStart,
    Syscall_Brk,
    Syscall_Nanosleep,
    Syscall_Clone,
    Syscall_Fork,

    Syscall_GetIPCMessageFrom,
    Syscall_PostIPCMessage,

    Syscall_SetProperty,
    Syscall_GetProperty,

    Syscall_ManageSharedMemoryKey, //region size, flags, existing key, return key/error
    Syscall_SharedMemoryKeyAction, //key to apply, flag to apply/remove, return virtual address

    Syscall_Execve,


    Syscall_R0_MemoryMap,               //Kernel level memory map that allows direct access to the kernel arbitrary memory mapping functions
    Syscall_R0_Fork,                    //Forks the process
    Syscall_R0_GetProcessInfo,          //Get the kernel's process information
    Syscall_R0_SetProcessInfo,          //Set the kernel's process information
    Syscall_R0_CreateThread,            //Create a new thread
    Syscall_R0_WipeMemoryMap,           //Wipe the memory map of a process
    Syscall_R0_GetInitrd,               //Map the initrd as readonly into the requesting process
    Syscall_R0_FreeInitrd,              //Free the initrd's memory

    Syscall_R1_PowerManagementEvent,    //Send or receive power management events

    Syscall_NumEnd = Syscall_Execve,
} SyscallFunctions;

#endif