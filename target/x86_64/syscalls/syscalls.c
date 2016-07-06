#include "managers.h"
#include "syscalls.h"

#include "libs/libCardinal/include/syscall.h"


void
SetupSecurityMonitor(void) {
    RegisterSyscall(Syscall_IOPL, SecurityMonitor_IOPL);
    RegisterSyscall(Syscall_RequestIRQPermissions, SecurityMonitor_SyscallReceived);
    RegisterSyscall(Syscall_RequestSystemPermissions, SecurityMonitor_SyscallReceived);
}

uint64_t
SecurityMonitor_IOPL(uint64_t UNUSED(instruction_pointer),
                     uint64_t syscall_num,
                     uint64_t *syscall_params) {
    SyscallData *syscall_data = (SyscallData*)syscall_params;

    ProcessInformation *procInfo;
    if(GetProcessReference(GetCurrentProcessUID(), &procInfo))
        return ENOTSUP;

    if(procInfo->SyscallFlags & ProcessSyscallFlags_PermissionsLocked)
        return EACCES;

    if(syscall_data->param_num != 1)
        return EINVAL;

    if(syscall_data->params[0] != 3 && syscall_data->params[0] != 0)
        return EINVAL;

    if(syscall_num == Syscall_IOPL) {
        uint64_t rflag = GetRFLAGS();
        rflag = rflag & ~(3 << 12);

        SetRFLAGS(rflag | (syscall_data->params[0] << 12));
    } else
        return ENOSYS;

    return 0;
}

uint64_t
SecurityMonitor_SyscallReceived(uint64_t UNUSED(instruction_pointer),
                                uint64_t syscall_num,
                                uint64_t *syscall_params) {
    SyscallData *syscall_data = (SyscallData*)syscall_params;

    ProcessInformation *procInfo;
    if(GetProcessReference(GetCurrentProcessUID(), &procInfo))
        return ENOTSUP;

    if(procInfo->SyscallFlags & ProcessSyscallFlags_PermissionsLocked)
        return EACCES;

    switch(syscall_num) {
    case Syscall_RequestIRQPermissions: {
        if(syscall_data->param_num != 0)
            return EINVAL;

        procInfo->Permissions |= ProcessPermissions_IRQAccess;
    }
    break;
    case Syscall_RequestSystemPermissions: {
        if(syscall_data->param_num != 0)
            return EINVAL;

        procInfo->Permissions |= ProcessPermissions_SystemPermissions;
    }
    break;
    default:
        return ENOSYS;
        break;
    }
    return 0;
}