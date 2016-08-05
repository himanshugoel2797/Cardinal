#include "libs/libCardinal/include/syscall.h"
#include "types.h"
#include "thread.h"
#include "memory.h"

#include "libs/libc/include/asm/prctl.h"

uint64_t
ArchPrctl_Syscall(uint64_t target,
                  uint64_t val) {


    if(target == ARCH_SET_GS) SetGSBase((void*)val);
    else if(target == ARCH_SET_FS) SetFSBase((void*)val);
    else if(target == ARCH_GET_GS) {
        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), val, NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return -EINVAL;

        uint64_t* gs_base_target = (uint64_t*)val;
        *gs_base_target = (uint64_t)GetGSBase();
    } else if(target == ARCH_GET_FS) {
        MemoryAllocationFlags flags = 0;
        CheckAddressPermissions(GetActiveVirtualMemoryInstance(), val, NULL, &flags);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User))
            return -EINVAL;


        uint64_t* fs_base_target = (uint64_t*)val;
        *fs_base_target = (uint64_t)GetFSBase();
    }

    return 0;
}
