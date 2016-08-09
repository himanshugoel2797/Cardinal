#include "syscalls_all.h"
#include "memory.h"
#include "libs/libCardinal/include/syscall.h"

uint64_t
ManageSharedMemoryKey_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {

	if(syscall_num != Syscall_ManageSharedMemoryKey)
		return -ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;

	if(data->param_num != 3)
		return -EINVAL;

	size_t len = data->params[0];
	uint64_t flags = data->params[1];
	uint64_t key = data->params[2];

	return ManageSharedMemoryKey(len, flags, key);
}

uint64_t
SharedMemoryKeyAction_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
	if(syscall_num != Syscall_SharedMemoryKeyAction)
		return -ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;

	if(data->param_num != 2)
		return -EINVAL;

	uint64_t key = data->params[0];
	uint64_t flags = data->params[1];

	return SharedMemoryKeyAction(key, flags);
}