#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "kmalloc.h"
#include "common.h"
#include "unicode.h"
#include "managers.h"

uint64_t
RegisterDriver_Syscall(uint64_t UNUSED(instruction_pointer),
                       uint64_t syscall_num,
                       uint64_t *syscall_params) {
    if(syscall_num != Syscall_RegisterDriver)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 3)
        return EINVAL;

    ProcessInformation *pInfo;
    GetProcessReference(GetCurrentProcessUID(), &pInfo);

    if(pInfo->Permissions & ProcessSyscallFlags_PermissionsLocked)
        return EPERM;

    if(pInfo->Permissions & ProcessSyscallFlags_DriverPermissions)
        return ENOSYS;

    strcpy_s(pInfo->Name, MAX_PROCESS_NAME_LEN,(const char*)data->params[0], MAX_PROCESS_NAME_LEN);
    if(!IsUTF8(pInfo->Name, MAX_PROCESS_NAME_LEN))
        strcpy(pInfo->Name, "Unknown Driver");

    int fds[2];
    if(CreateAnonPipe(FileDescriptorFlags_CloseOnExec, fds) == 0)
        return fds[0];

    return 0;
}