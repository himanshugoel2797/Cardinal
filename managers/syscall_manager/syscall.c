#include "syscall.h"
#include "synchronization.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "common.h"

static uint64_t free_syscall_index = 0;
static Spinlock syscall_lock;
SyscallHandler Syscalls[MAX_SYSCALL_COUNT];


void
SyscallMan_Initialize(void) {
    syscall_lock = CreateSpinlock();
}

uint64_t
SyscallReceived(uint64_t instruction_pointer,
                uint64_t syscall_num,
                uint64_t *syscall_params) {

    /*First thing to do is make the page the syscall_param points to read only
      informing the page fault handler to stall other cores if they attempt
      to write to the region of memory being protected.

      Copy the parameters over to a kernel buffer, then disable the protection.
      Now function only with the kernel buffer
    */

    SyscallData *data = (SyscallData*)syscall_params;

    MemoryAllocationFlags flags = 0;
    CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)syscall_params, NULL, &flags);

    if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
        return SyscallError_InvalidParameters;

    SyscallData k_data;
    memcpy(&k_data, data, sizeof(SyscallData));


    flags = 0;
    CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)k_data.params, NULL, &flags);

    if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
        return SyscallError_InvalidParameters;

    if(k_data.param_num > MAX_PARAM_COUNT)
        return SyscallError_InvalidParameters;

    if(k_data.size != sizeof(SyscallData))
        return SyscallError_InvalidParameters;


    //Create a kernel side copy of the oarameters
    uint64_t k_data_param[MAX_PARAM_COUNT];
    for(uint64_t i = 0; i < k_data.param_num; i++)
        k_data_param[i] = k_data.params[i];
    k_data.params = k_data_param;

    uint32_t syscall_baseNum = (uint32_t)syscall_num;

    if((syscall_baseNum < Syscall_NumStart) | (syscall_baseNum > Syscall_NumEnd))
        return SyscallError_NoSyscall;


    if(Syscalls[syscall_baseNum] != NULL)
    {
        uint64_t retVal = Syscalls[syscall_baseNum](instruction_pointer,
                                                    syscall_num,
                                                    (uint64_t*)&k_data);

        return retVal;
    }
    return SyscallError_NoSyscall;
}

uint64_t
AllocateSyscall(void) {
    LockSpinlock(syscall_lock);
    uint64_t ret = free_syscall_index++;
    UnlockSpinlock(syscall_lock);
    return ret;
}

void
RegisterSyscall(uint64_t syscall_num,
                SyscallHandler handler) {
    if(syscall_num < MAX_SYSCALL_COUNT) {
        LockSpinlock(syscall_lock);
        Syscalls[syscall_num] = handler;
        UnlockSpinlock(syscall_lock);
    }
}