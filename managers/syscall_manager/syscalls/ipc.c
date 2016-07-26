#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/file_server.h"
#include "managers.h"
#include "common.h"
#include "kmalloc.h"
#include "synchronization.h"

uint64_t
GetIPCMessage_Syscall(uint64_t UNUSED(instruction_pointer),
					  uint64_t syscall_num,
					  uint64_t *syscall_params) {
	if(syscall_num != Syscall_GetIPCMessage)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 1)
		return EINVAL;

	__asm__ ("cli\n\thlt" :: "a"(data->params[0]));
	return GetMessage((Message*)data->params[0]);
}

uint64_t
GetIPCMessageFrom_Syscall(uint64_t UNUSED(instruction_pointer),
					  uint64_t syscall_num,
					  uint64_t *syscall_params) {
	if(syscall_num != Syscall_GetIPCMessageFrom)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 2)
		return EINVAL;

	return GetMessageFrom((Message*)data->params[0], (UID)data->params[1]);	
}

uint64_t
PostIPCMessage_Syscall(uint64_t UNUSED(instruction_pointer),
					  uint64_t syscall_num,
					  uint64_t *syscall_params) {
	if(syscall_num != Syscall_PostIPCMessage)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 1)
		return EINVAL;

	return PostMessage((Message*)data->params[0]);
}