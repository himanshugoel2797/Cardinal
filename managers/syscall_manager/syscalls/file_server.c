#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "managers.h"
#include "common.h"


uint64_t
Open_Syscall(uint64_t UNUSED(instruction_pointer),
             uint64_t syscall_num,
             uint64_t *syscall_params) {
	if(syscall_num != Syscall_Open)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 3)
		return EINVAL;

	return 0;
}

uint64_t
Close_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params) {
	if(syscall_num != Syscall_Close)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 1)
		return EINVAL;

	return 0;
}

uint64_t
Read_Syscall(uint64_t UNUSED(instruction_pointer),
             uint64_t syscall_num,
             uint64_t *syscall_params) {
	if(syscall_num != Syscall_Read)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 3)
		return EINVAL;

	return 0;
}

uint64_t
Write_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params) {
	if(syscall_num != Syscall_Write)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 3)
		return EINVAL;

	return 0;
}