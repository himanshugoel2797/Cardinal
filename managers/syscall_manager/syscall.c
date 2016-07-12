#include "syscall.h"
#include "synchronization.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "target/hal/syscall.h"
#include "common.h"
#include "syscalls/syscalls_all.h"

static uint64_t free_syscall_index = 0;
static Spinlock syscall_lock;
SyscallHandler Syscalls[MAX_SYSCALL_COUNT];


void
SyscallMan_Initialize(void) {
    syscall_lock = CreateSpinlock();
    for(int i = 0; i < MAX_SYSCALL_COUNT; i++)Syscalls[i] = NULL;
    RegisterAllSyscalls();
}

uint64_t
SyscallReceived(uint64_t instruction_pointer,
                uint64_t syscall_num,
                uint64_t *syscall_params,
                uint64_t syscall_param_cnt) {

    /*
    Copy the parameters over to a kernel buffer, Now function only with the kernel buffer
    */

    MemoryAllocationFlags flags = 0;
    CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)syscall_params, NULL, &flags);

    if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
        return EINVAL;

    SyscallData k_data;
    k_data.params = syscall_params;
    k_data.param_num = syscall_param_cnt;

    if(k_data.param_num > MAX_PARAM_COUNT)
        return EINVAL;

    //Create a kernel side copy of the oarameters
    uint64_t k_data_param[MAX_PARAM_COUNT];
    for(uint64_t i = 0; i < k_data.param_num; i++)
        k_data_param[i] = k_data.params[i];
    k_data.params = k_data_param;

    uint32_t syscall_baseNum = (uint32_t)syscall_num;

    if((syscall_baseNum < Syscall_NumStart) | (syscall_baseNum > Syscall_NumEnd))
        return ENOSYS;

    if(Syscalls[syscall_baseNum] != NULL) {
        uint64_t retVal = Syscalls[syscall_baseNum](instruction_pointer,
                          syscall_num,
                          (uint64_t*)&k_data);

        return retVal;
    } else __asm__ ("cli\n\thlt" :: "a"(syscall_num), "b"(instruction_pointer));

    return ENOSYS;
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