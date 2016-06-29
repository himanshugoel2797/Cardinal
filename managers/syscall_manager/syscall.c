#include "syscall.h"
#include "synchronization.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"

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

    flags = 0;
    CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)data->params, NULL, &flags);

    if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
        return SyscallError_InvalidParameters;

    if(data->param_num > MAX_PARAM_COUNT)
        return SyscallError_InvalidParameters;

    if(data->size != sizeof(SyscallData))
        return SyscallError_InvalidParameters;

    uint32_t syscall_baseNum = (uint32_t)syscall_num;
    uint32_t syscall_functionNum = (uint32_t)(syscall_num >> 32);

    if((syscall_baseNum < Syscall_NumStart) | (syscall_baseNum > Syscall_NumEnd))
        return SyscallError_NoSyscall;


    if(Syscalls[syscall_baseNum] != NULL)
    {
        //Lock the page
        uint64_t key0 = LockPageToUser((uint64_t)data);
        uint64_t key1 = LockPageToUser((uint64_t)data->params);

        uint64_t key2 = 0;
        if( ((uint64_t)&data->params[data->param_num - 1])/PAGE_SIZE !=  ((uint64_t)data->params)/PAGE_SIZE)
        {
            key2 = LockPageToUser((uint64_t)&data->params[data->param_num - 1]);
        }

        uint64_t retVal = Syscalls[syscall_baseNum](instruction_pointer,
                                                    syscall_functionNum,
                                                    syscall_params);

        if(key2 != 0)
            UnlockPageToUser((uint64_t)&data->params[data->param_num - 1], key2);

        UnlockPageToUser((uint64_t)data->params, key1);
        UnlockPageToUser((uint64_t)data, key0);

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