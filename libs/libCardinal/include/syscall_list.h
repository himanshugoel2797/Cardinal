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

    Syscall_NumEnd = Syscall_GetProperty,
} SyscallFunctions;

#endif