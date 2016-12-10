#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"

void
RegisterAllSyscalls(void) {

    MemoryInitLocks();
    PropertyInitLocks();

    RegisterSyscall(Syscall_Nanosleep, Nanosleep_Syscall);
    RegisterSyscall(Syscall_Brk, Brk_Syscall);

    RegisterSyscall(Syscall_GetIPCMessageFrom, GetIPCMessageFrom_Syscall);
    RegisterSyscall(Syscall_PostIPCMessage, PostIPCMessage_Syscall);

    RegisterSyscall(Syscall_SetProperty, SetProperty_Syscall);
    RegisterSyscall(Syscall_GetProperty, GetProperty_Syscall);

    RegisterSyscall(Syscall_GetErrno, GetErrno_Syscall);

    RegisterSyscall(Syscall_R0_Map, R0Map_Syscall);
    RegisterSyscall(Syscall_R0_Unmap, R0Unmap_Syscall);

    RegisterSyscall(Syscall_Unmap, Unmap_Syscall);

    RegisterSyscall(Syscall_R0_AllocatePages, R0AllocatePages_Syscall);
    RegisterSyscall(Syscall_R0_FreePages, R0FreePages_Syscall);

    RegisterSyscall(Syscall_R0_CreateProcess, R0CreateProcess_Syscall);
    RegisterSyscall(Syscall_R0_StartProcess, R0StartProcess_Syscall);
    RegisterSyscall(Syscall_R0_CreateThread, R0CreateThread_Syscall);
    RegisterSyscall(Syscall_R0_GetBootInfo, R0GetBootInfo_Syscall);

    RegisterSyscall(Syscall_R0_GetPhysicalAddress, R0GetPhysicalAddress_Syscall);

    RegisterSyscall(Syscall_R0_AllocateSharedMemory, R0AllocateSharedMemory_Syscall);

    RegisterSyscall(Syscall_AllocateSharedMemory, AllocateSharedMemory_Syscall);
    RegisterSyscall(Syscall_GetSharedMemoryKey, GetSharedMemoryKey_Syscall);
    RegisterSyscall(Syscall_ApplySharedMemoryKey, ApplySharedMemoryKey_Syscall);
    RegisterSyscall(Syscall_FreeSharedMemoryKey, FreeSharedMemoryKey_Syscall);
    RegisterSyscall(Syscall_GetSharedMemoryKeyUsageCount, GetSharedMemoryKeyUsageCount_Syscall);

    RegisterSyscall(Syscall_WaitForMessage, WaitForMessage_Syscall);
    RegisterSyscall(Syscall_GetIPCMessageMsgType, GetIPCMessageMsgType_Syscall);
}