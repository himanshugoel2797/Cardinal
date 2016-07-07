#include "libs/libCardinal/include/syscall.h"
#include "types.h"
#include "thread.h"
#include "memory.h"

#include <asm/prctl.h>

uint64_t
ArchPrctl_Syscall(uint64_t UNUSED(instruction_pointer),
                  uint64_t syscall_num,
                  uint64_t *syscall_params) {
    if(syscall_num != Syscall_ArchPrctl)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2)
        return EINVAL;

    if(data->params[0] == ARCH_SET_GS) SetGSBase((void*)data->params[1]);
    else if(data->params[0] == ARCH_SET_FS) SetFSBase((void*)data->params[1]);
    else if(data->params[0] == ARCH_GET_GS) {
        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)data->params[1], NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return EINVAL;

        uint64_t* gs_base_target = (uint64_t*)data->params[1];
        *gs_base_target = (uint64_t)GetGSBase();
    } else if(data->params[0] == ARCH_GET_FS) {
        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), (uint64_t)data->params[1], NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return EINVAL;


        uint64_t* fs_base_target = (uint64_t*)data->params[1];
        *fs_base_target = (uint64_t)GetFSBase();
    }

    return 0;
}
