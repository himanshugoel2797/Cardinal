#include "syscalls_all.h"
#include "syscalls/arch_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/syscall_property.h"

#include "libs/libc/include/thread.h"
#include "libs/libc/include/asm/prctl.h"

#include "managers.h"

uint64_t
Execve_Syscall(uint64_t UNUSED(instruction_pointer),
               uint64_t syscall_num,
               uint64_t *syscall_params) {
    if(syscall_num != Syscall_Execve)
        return -ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    //Arg 0 = pointer to the elf data
    //Arg 1 = elf data size
    //Arg 2 = arguments to the file
    //Arg 3 = environment variables

    if(data->param_num != 4)
        return -EINVAL;

    //Copy the elf data into the kernel, clear the user mode mappings
    //Load the elf and jump to its entry point
}