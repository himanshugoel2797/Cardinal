#include "managers.h"
#include "syscalls.h"

#include "libs/libCardinal/include/syscall.h"


void
SetupSecurityMonitor(void)
{
	RegisterSyscall(Syscall_SecurityMonitor, SecurityMonitor_SyscallReceived);
}

uint64_t
SecurityMonitor_SyscallReceived(uint64_t UNUSED(instruction_pointer),
                uint64_t syscall_num,
                uint64_t *syscall_params)
{
	syscall_num = syscall_num >> 32;
	SyscallData *syscall_data = (SyscallData*)syscall_params;

	ProcessInformation *procInfo;
	if(GetProcessReference(GetCurrentProcessUID(), &procInfo))
		return SyscallError_Unknown;

	if(procInfo->SyscallFlags & ProcessSyscallFlags_PermissionsLocked)
		return SyscallError_NoPermissions;

	switch(syscall_num)
	{
		case SyscallFunction_SecurityMonitor_RequestIOPermissions:
		{
			if(syscall_data->param_num != 0)
				return SyscallError_TooManyParameters;
			
			SetRFLAGS(GetRFLAGS() | (3 << 12));			
		}
		break;
		case SyscallFunction_SecurityMonitor_RequestIRQPermissions:
		{
			if(syscall_data->param_num != 0)
				return SyscallError_TooManyParameters;

			procInfo->Permissions |= ProcessPermissions_IRQAccess;
		}
		break;
		case SyscallFunction_SecurityMonitor_RequestSystemPermissions:
		{
			if(syscall_data->param_num != 0)
				return SyscallError_TooManyParameters;

			procInfo->Permissions |= ProcessPermissions_SystemPermissions;
		}
		break;
		default:
			return SyscallError_NoSyscallFunction;
		break;
	}
	return SyscallError_None;
}