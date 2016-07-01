#ifndef _CARDINAL_LIB_SYSCALL_H_
#define _CARDINAL_LIB_SYSCALL_H_

#include <stdint.h>
#include <stddef.h>

#define MAX_PARAM_COUNT 10

typedef enum {
    SyscallError_None = 0,
    SyscallError_Unknown,
    SyscallError_NoSyscall,
    SyscallError_NoSyscallFunction,
    SyscallError_TooManyParameters,
    SyscallError_InvalidParameters,
    SyscallError_NoPermissions
} SyscallError;

typedef enum {
    Syscall_NumStart = 0,
    Syscall_SecurityMonitor = 0,
    Syscall_Memory = 1,
    Syscall_Exec = 2,
    Syscall_Thread = 3,
    Syscall_Signal = 4,
    Syscall_IO = 5,
    Syscall_Sibyl = 6,
    Syscall_NumEnd = Syscall_Sibyl
} SyscallsNumbers;

typedef enum {
    SyscallFunction_SecurityMonitor_RequestIOPermissions = 1,
    SyscallFunction_SecurityMonitor_RequestIRQPermissions = 2,
    SyscallFunction_SecurityMonitor_RequestSystemPermissions = 3,

    SyscallFunction_Memory_Sbrk = 0,
    SyscallFunction_Memory_MMap = 1,

    SyscallFunction_Exec_Execve = 0,
    SyscallFunction_Exec_Fork = 1,
    SyscallFunction_Exec_Exit = 2,
    SyscallFunction_Exec_GetPID = 3,

    SyscallFunction_Thread_Create = 0,
    SyscallFunction_Thread_Kill = 1,
    SyscallFunction_Thread_GetTID = 3,

    SyscallFunction_Signal_SetHandler = 0,
    SyscallFunction_Signal_SetThreadMask = 1,
    SyscallFunction_Signal_Raise = 2,

    SyscallFunction_IO_Create = 0,
    SyscallFunction_IO_Open = 1,
    SyscallFunction_IO_Close = 2,
    SyscallFunction_IO_Read = 3,
    SyscallFunction_IO_Write = 4,
    SyscallFunction_IO_FStat = 5,
    SyscallFunction_IO_DirOpen = 6,
    SyscallFunction_DirStat = 7,
    SyscallFunction_IO_Remove = 8,
    SyscallFunction_IO_RemoveDir = 9,
    SyscallFunction_IO_CreateDir = 10,

    SyscallFunction_Sibyl_SubmitSyscallFilter = 0
} SyscallFunctions;

typedef struct {
    uint64_t size;
    uint64_t *params;
    uint64_t param_num;
} SyscallData;

inline SyscallError
Syscall(uint32_t syscall_num,
        uint32_t syscall_func_num,
        const uint64_t *params,
        uint64_t param_count,
        void *returnData) {
    if(params == NULL)return SyscallError_InvalidParameters;
    if(param_count > MAX_PARAM_COUNT)return SyscallError_TooManyParameters;

    uint64_t parameters[MAX_PARAM_COUNT];
    for(uint64_t i = 0; i < param_count; i++)
        parameters[i] = params[i];

    SyscallData data;
    data.size = sizeof(SyscallData);
    data.params = parameters;
    data.param_num = param_count;

    SyscallError ret_error = SyscallError_Unknown;


#if defined(x86_64)
    uint64_t syscall_val = syscall_func_num;
    syscall_val = syscall_val << 32 | syscall_num;
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(&data)
    );
#endif

    if(returnData != NULL)
        returnData = (void*)data.params;

    return ret_error;
}

#endif