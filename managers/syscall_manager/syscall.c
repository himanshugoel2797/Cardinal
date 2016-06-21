#include "syscall.h"
#include "synchronization.h"

static uint64_t free_syscall_index = 0;
static Spinlock syscall_lock;
SyscallHandler Syscalls[MAX_SYSCALL_COUNT];


void
SyscallMan_Initialize(void)
{
	syscall_lock = CreateSpinlock();
}

void
SyscallReceived(uint64_t instruction_pointer,
                uint64_t syscall_num,
                uint64_t *syscall_params) {


    instruction_pointer = 0;
    syscall_num = 0;
    syscall_params = NULL;

    /*First thing to do is make the page the syscall_param points to read only
      informing the page fault handler to stall other cores if they attempt
      to write to the region of memory being protected.

      Copy the parameters over to a kernel buffer, then disable the protection.
      Now function only with the kernel buffer
    */
    while(1) __asm__ ("cli\n\thlt" :: "a"(syscall_num));
}

uint64_t
AllocateSyscall(void)
{
	LockSpinlock(syscall_lock);
	uint64_t ret = free_syscall_index++;
	UnlockSpinlock(syscall_lock);
	return ret;
}

void
RegisterSyscall(uint64_t syscall_num,
				SyscallHandler handler)
{
	if(syscall_num < MAX_SYSCALL_COUNT)
	{
		LockSpinlock(syscall_lock);
		Syscalls[syscall_num] = handler;
		UnlockSpinlock(syscall_lock);
	}
}