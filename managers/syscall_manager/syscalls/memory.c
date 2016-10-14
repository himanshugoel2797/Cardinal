#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "kmalloc.h"
#include <sys/mman.h>

uint64_t
brk(void* targ_brk_address) {
    ProcessInformation *p_info;
    GetProcessReference(GetCurrentProcessUID(), &p_info);

    if(targ_brk_address == NULL) {
        uint64_t addr = 0;
        FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                               &addr,
                               PAGE_SIZE,
                               MemoryAllocationType_Heap,
                               MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

        LockSpinlock(p_info->lock);
        if(p_info->HeapBreak == 0)p_info->HeapBreak = addr;
        else addr = p_info->HeapBreak;
        UnlockSpinlock(p_info->lock);

        return addr;
    }

    uint64_t area_base = GetMemoryAllocationTypeBase(MemoryAllocationType_Heap, MemoryAllocationFlags_User);
    uint64_t area_top = GetMemoryAllocationTypeTop(MemoryAllocationType_Heap, MemoryAllocationFlags_User);

    if(area_base <= (uint64_t)targ_brk_address && area_top > (uint64_t)targ_brk_address) {
        //TODO expand the heap by a few pages and return the new heap break

        LockSpinlock(p_info->lock);

        if((uint64_t)targ_brk_address <= p_info->HeapBreak)
            return UnlockSpinlock(p_info->lock), (uint64_t)targ_brk_address;

        uint64_t size = (uint64_t)targ_brk_address - p_info->HeapBreak;
        uint64_t prev_heap_break = p_info->HeapBreak;

        if(size % PAGE_SIZE != 0)size += PAGE_SIZE - (size % PAGE_SIZE);
        p_info->HeapBreak += size;

        MapPage(GetActiveVirtualMemoryInstance(),
                AllocatePhysicalPageCont(size/PAGE_SIZE),
                prev_heap_break,
                size,
                CachingModeWriteBack,
                MemoryAllocationType_Heap,
                MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

        UnlockSpinlock(p_info->lock);

        return (uint64_t)targ_brk_address;
    }

    return -ENOMEM;

}

uint64_t
Brk_Syscall(uint64_t UNUSED(instruction_pointer),
            uint64_t syscall_num,
            uint64_t *syscall_params) {
    if(syscall_num != Syscall_Brk)
        return -ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;


    if(data->param_num != 1)
        return -ENOSYS;

    void* targ_brk_address = (void*)data->params[0];
    return brk(targ_brk_address);
}

uint64_t
R0MemoryMap_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_MemoryMap)
        return -ENOSYS;

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0)
        return -EPERM;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1)
        return -ENOSYS;

    struct MemoryMapParams *mmap_params = (struct MemoryMapParams*)data->params[0];

    ProcessInformation *p_info;
    if(GetProcessReference(mmap_params->TargetPID, &p_info) != ProcessErrors_None)
        return -EINVAL;

    //Prevent any attempts to map into kernel space
    mmap_params->AllocationFlags |= MemoryAllocationFlags_User;

    if(mmap_params->VirtualAddress == 0) {
        if(FindFreeVirtualAddress(p_info->PageTable,
                                  &mmap_params->VirtualAddress,
                                  mmap_params->Length,
                                  mmap_params->AllocationType,
                                  mmap_params->AllocationFlags) != MemoryAllocationErrors_None)

            return -ENOMEM;
    }

    if(MapPage(p_info->PageTable,
               mmap_params->PhysicalAddress,
               mmap_params->VirtualAddress,
               mmap_params->Length,
               mmap_params->CacheMode,
               mmap_params->AllocationType,
               mmap_params->AllocationFlags) != MemoryAllocationErrors_None)
        return -ENOMEM;
    else
        return mmap_params->VirtualAddress;
}