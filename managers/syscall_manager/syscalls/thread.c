#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"

#include "managers.h"

uint64_t
set_tid_address(void* address)
{
  Thread_SetClearChildTIDAddress(GetCurrentThreadUID(),
                                 address);

  return GetCurrentThreadUID();
}

uint64_t
SetTidAddress_Syscall(uint64_t UNUSED(instruction_pointer),
                      uint64_t syscall_num,
                      uint64_t *syscall_params) {
    if(syscall_num != Syscall_SetTidAddress)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1)
        return EINVAL;

    return set_tid_address((void*)data->params[0]);
}