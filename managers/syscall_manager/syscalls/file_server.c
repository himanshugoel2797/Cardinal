#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libCardinal/include/file_server.h"
#include "managers.h"
#include "common.h"
#include "kmalloc.h"
#include "synchronization.h"

static UID file_server_pid;
static Spinlock io_lock = NULL;

void
SetFileserverPID(UID pid) {
	file_server_pid = pid;
	if(io_lock == NULL)io_lock = CreateSpinlock();
}

uint64_t
Open_Syscall(uint64_t UNUSED(instruction_pointer),
             uint64_t syscall_num,
             uint64_t *syscall_params) {
	if(syscall_num != Syscall_Open)
		return ENOSYS;

	SyscallData *data = (SyscallData*)syscall_params;
	if(data->param_num != 3)
		return EINVAL;

	Message *m = kmalloc(sizeof(Message));
	m->DestinationPID = file_server_pid;
	m->SourcePID = GetCurrentProcessUID();

	struct OpenRequest *oreq = (struct OpenRequest*)m->Content;
	oreq->flags = data->params[1];
	oreq->mode = data->params[2];
	oreq->path_len = strnlen_s((const char*)data->params[0], MAX_PATH_LEN);
	strcpy_s(oreq->path, MAX_PATH_LEN, (const char*)data->params[0], oreq->path_len); 
	if(oreq->path[MAX_PATH_LEN - 1] != '\0')
	{
		kfree(m);
		return ENAMETOOLONG;
	}

	while(!PostMessage(m));
	YieldThread();

	while(!GetMessageFrom(m, file_server_pid))
		YieldThread();

	uint64_t retVal = ((uint64_t*)m->Content)[0];
	kfree(m);
	return retVal;
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

	Message *m = kmalloc(sizeof(Message));
	m->DestinationPID = file_server_pid;
	m->SourcePID = GetCurrentProcessUID();

	uint64_t *dat = (uint64_t*)m->Content;
	dat[0] = syscall_num;

	memcpy(&dat[1], data->params, sizeof(uint64_t) * data->param_num);

	while(!PostMessage(m));
	YieldThread();

	while(!GetMessageFrom(m, file_server_pid))
		YieldThread();

	uint64_t retVal = dat[0];
	kfree(m);
	return retVal;
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

	Message *m = kmalloc(sizeof(Message));
	m->DestinationPID = file_server_pid;
	m->SourcePID = GetCurrentProcessUID();

	uint64_t *dat = (uint64_t*)m->Content;
	dat[0] = syscall_num;

	memcpy(&dat[1], data->params, sizeof(uint64_t) * data->param_num);

	while(!PostMessage(m));
	YieldThread();

	while(!GetMessageFrom(m, file_server_pid))
		YieldThread();

	uint64_t retVal = dat[0];
	kfree(m);
	return retVal;
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

	Message *m = kmalloc(sizeof(Message));
	m->DestinationPID = file_server_pid;
	m->SourcePID = GetCurrentProcessUID();

	uint64_t *dat = (uint64_t*)m->Content;
	dat[0] = syscall_num;

	memcpy(&dat[1], data->params, sizeof(uint64_t) * data->param_num);

	while(!PostMessage(m));
	YieldThread();

	while(!GetMessageFrom(m, file_server_pid))
		YieldThread();

	uint64_t retVal = dat[0];
	kfree(m);
	return retVal;
}