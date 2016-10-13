#ifndef _CARDINAL_MEMORY_ENUMS_H_
#define _CARDINAL_MEMORY_ENUMS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"
#include "bootinfo.h"
#include "ipc.h"

///Memory Caching Modes
typedef enum {
    CachingModeWriteBack = 0,   ///<Write Back
    CachingModeUncachable = 1,  ///<Uncachable
    CachingModeWriteThrough = 2 ///<Write Through
} CachingMode;

///Memory Allocation Type
typedef enum {
    MemoryAllocationType_Heap = 0,                          ///<Heap
    MemoryAllocationType_Stack = (1 << 0),                  ///<Stack
    MemoryAllocationType_MMap = (1 << 1),                   ///<Memory Mapped
    MemoryAllocationType_Fork = (1 << 2),                   ///<Forked
    MemoryAllocationType_Paged = (1 << 3),                  ///<Paged from Disk
    MemoryAllocationType_Shared = (1 << 4),                 ///<Shared Memory
    MemoryAllocationType_Global = (1 << 5),                 ///<Shared between all cores
    MemoryAllocationType_Application = (1 << 6),            ///<Process related Read Only information, removed on Execve
    MemoryAllocationType_MMapLo = (1 << 7),                 ///<Memory Mapped (32 bit address)
    MemoryAllocationType_ApplicationProtected = (1 << 8)    ///<Process related user RW information, persists on Execve
} MemoryAllocationType;

///Memory Allocation Flags
typedef enum {
    MemoryAllocationFlags_NoExec = (0 << 1),  ///<Execute Disable
    MemoryAllocationFlags_Exec = (1 << 1),    ///<Execute Enable
    MemoryAllocationFlags_Read = 0,
    MemoryAllocationFlags_Write = (1 << 2),   ///<Write Enabled
    MemoryAllocationFlags_Kernel = 0,         ///<Kernel Access Permission
    MemoryAllocationFlags_User = (1 << 3),    ///<User Access Permission
    MemoryAllocationFlags_Present = (1 << 4),
    MemoryAllocationFlags_NotPresent = 0
} MemoryAllocationFlags;

///Memory Allocation Errors
typedef enum {
    MemoryAllocationErrors_None = 0,                          ///<No Error
    MemoryAllocationErrors_AlreadyMapped = (1 << 0),          ///<Memory is already mapped
    MemoryAllocationErrors_NotMapped = (1 << 1),              ///<Memory is not mapped
    MemoryAllocationErrors_PartiallyMapped = (1 << 2),        ///<Memory is partially mapped
    MemoryAllocationErrors_InvalidFlags = (1 << 3),           ///<The Flags parameter is invalid
    MemoryAllocationErrors_InvalidVirtualAddress = (1 << 4),  ///<Invalid Virtual Address
    MemoryAllocationErrors_InvalidPhysicalAddress = (1 << 5), ///<Invalid Physical Address
    MemoryAllocationErrors_Unknown = (1 << 6)                 ///<Unknown Error
} MemoryAllocationErrors;

struct MemoryMapParams {
    UID TargetPID;
    uint64_t PhysicalAddress;
    uint64_t VirtualAddress;
    uint64_t Length;
    CachingMode CacheMode;
    MemoryAllocationType AllocationType;
    MemoryAllocationFlags AllocationFlags;
};


#ifndef _KERNEL_

static __inline uint64_t
R0_MemoryMap(UID pid,
             uint64_t phys,
             uint64_t virt,
             uint64_t len,
             CachingMode cacheMode,
             MemoryAllocationType type,
             MemoryAllocationFlags flags) {
    struct MemoryMapParams p;


    p.TargetPID = pid;
    p.PhysicalAddress = phys;
    p.VirtualAddress = virt;
    p.Length = len;
    p.CacheMode = cacheMode;
    p.AllocationType = type;
    p.AllocationFlags = flags;

    return Syscall1(Syscall_R0_MemoryMap, &p);
}

static __inline int
R0_GetBootInfo(CardinalBootInfo *bInfo) {
    return (int)GetProperty(CardinalProperty_R0_BootInfo, (uint64_t)bInfo);
}

static __inline uint64_t
R0_GetPhysicalAddress(uint64_t v_addr) {
    return GetProperty(CardinalProperty_R0_PhysicalAddress, v_addr);
}

#endif

#endif