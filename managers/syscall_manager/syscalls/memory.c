#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "kmalloc.h"
#include <sys/mman.h>

uint64_t
Brk_Syscall(uint64_t UNUSED(instruction_pointer),
            uint64_t syscall_num,
            uint64_t *syscall_params) {
    
    if(syscall_num != Syscall_Brk) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;


    if(data->param_num != 1) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    void* targ_brk_address = (void*)data->params[0];
    
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

        SyscallSetErrno(0);
        return addr;
    }

    uint64_t area_base = GetMemoryAllocationTypeBase(MemoryAllocationType_Heap, MemoryAllocationFlags_User);
    uint64_t area_top = GetMemoryAllocationTypeTop(MemoryAllocationType_Heap, MemoryAllocationFlags_User);

    if(area_base <= (uint64_t)targ_brk_address && area_top > (uint64_t)targ_brk_address) {
        //TODO expand the heap by a few pages and return the new heap break

        LockSpinlock(p_info->lock);

        if((uint64_t)targ_brk_address <= p_info->HeapBreak){

            UnlockSpinlock(p_info->lock), 
            
            SyscallSetErrno(0);
            return (uint64_t)targ_brk_address;
        }

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

        SyscallSetErrno(0);
        return (uint64_t)targ_brk_address;
    }

    SyscallSetErrno(-ENOMEM);
    return p_info->HeapBreak;
}

uint64_t
R0Map_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_Map) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    struct MemoryMapParams *mmap_params = (struct MemoryMapParams*)data->params[0];

    ProcessInformation *p_info;
    if(GetProcessReference(mmap_params->TargetPID, &p_info) != ProcessErrors_None){
        SyscallSetErrno(-EINVAL);
        return 0;
    }

    //Prevent any attempts to map into kernel space
    mmap_params->AllocationFlags |= MemoryAllocationFlags_User;

    if(mmap_params->VirtualAddress == 0) {
        if(FindFreeVirtualAddress(p_info->PageTable,
                                  &mmap_params->VirtualAddress,
                                  mmap_params->Length,
                                  mmap_params->AllocationType,
                                  mmap_params->AllocationFlags) != MemoryAllocationErrors_None)

        SyscallSetErrno(-ENOMEM);
        return 0;
    }

    if(MapPage(p_info->PageTable,
               mmap_params->PhysicalAddress,
               mmap_params->VirtualAddress,
               mmap_params->Length,
               mmap_params->CacheMode,
               mmap_params->AllocationType,
               mmap_params->AllocationFlags) != MemoryAllocationErrors_None) {

        SyscallSetErrno(-ENOMEM);
        return 0;
    }
    else {

        SyscallSetErrno(0);
        return mmap_params->VirtualAddress;
    }
}

uint64_t
R0Unmap_Syscall(uint64_t UNUSED(instruction_pointer),
                uint64_t syscall_num,
                uint64_t *syscall_params) {
    
    if(syscall_num != Syscall_R0_Unmap){
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0){
        SyscallSetErrno(-EPERM);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 3){
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    ProcessInformation *p_info;
    if(GetProcessReference(data->params[0], &p_info) != ProcessErrors_None){
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    UnmapPage(p_info->PageTable,
              data->params[1],
              data->params[2]);

    return SyscallSetErrno(0);
}