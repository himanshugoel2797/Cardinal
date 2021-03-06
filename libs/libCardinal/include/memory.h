/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_MEMORY_ENUMS_H_
#define _CARDINAL_MEMORY_ENUMS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"
#include "bootinfo.h"
#include "ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup memory_syscall Memory Syscalls
 * @{
 */

//! Memory Caching Modes.
typedef enum {
    CachingModeWriteBack = 0,   //!< Write Back.
    CachingModeUncachable = 1,  //!< Uncachable.
    CachingModeWriteThrough = 2 //!< Write Through.
} CachingMode;

//! Memory Allocation Type.
typedef enum {
    MemoryAllocationType_Heap = 0,                          //!< Heap.
    MemoryAllocationType_Stack = (1 << 0),                  //!< Stack.
    MemoryAllocationType_MMap = (1 << 1),                   //!< Memory Mapped.
    MemoryAllocationType_Fork = (1 << 2),                   //!< Forked.
    MemoryAllocationType_Paged = (1 << 3),                  //!< Paged from Disk.
    MemoryAllocationType_ReservedAllocation = (1 << 4),     //!< Physical backing memory is not allocated until page fault.
    MemoryAllocationType_Global = (1 << 5),                 //!< Shared between all cores.
    MemoryAllocationType_Application = (1 << 6),            //!< Process related Read Only information, removed on Execve.
    MemoryAllocationType_MMapLo = (1 << 7),                 //!< Memory Mapped (32 bit address).
    MemoryAllocationType_ReservedBacking = (1 << 8),        //!< Page was allocated passively by ReservedAllocation, must free as part of page table.
    MemoryAllocationType_Shared = (1 << 9),                 //!< Shared memory.
    MemoryAllocationType_Phys = (1 << 10),                  //!< Mapped region should not be automatically freed.
} MemoryAllocationType;

#ifdef _KERNEL_
#define MAX_ALLOCATION_TYPE_BIT 8

#else
#define PAGE_SIZE 4096
#endif

typedef enum {
    PhysicalMemoryAllocationFlags_None,
    PhysicalMemoryAllocationFlags_32Bit,
} PhysicalMemoryAllocationFlags;

//! Memory Allocation Flags.
typedef enum {
    MemoryAllocationFlags_NoExec = (0 << 1),  //!< Execute Disable.
    MemoryAllocationFlags_Exec = (1 << 1),    //!< Execute Enable.
    MemoryAllocationFlags_Read = 0,           //!< Read Only.
    MemoryAllocationFlags_Write = (1 << 2),   //!< Write Enabled.
    MemoryAllocationFlags_Kernel = (1 << 5),         //!< Kernel Access Permission.
    MemoryAllocationFlags_User = (1 << 3),    //!< User Access Permission.
    MemoryAllocationFlags_Present = (1 << 4), //!< Page is present.
    MemoryAllocationFlags_NotPresent = 0      //!< Page is not present.
} MemoryAllocationFlags;

//!Memory Allocation Errors.
typedef enum {
    MemoryAllocationErrors_None = 0,                          //!< No Error.
    MemoryAllocationErrors_AlreadyMapped = (1 << 0),          //!< Memory is already mapped.
    MemoryAllocationErrors_NotMapped = (1 << 1),              //!< Memory is not mapped.
    MemoryAllocationErrors_PartiallyMapped = (1 << 2),        //!< Memory is partially mapped.
    MemoryAllocationErrors_InvalidFlags = (1 << 3),           //!< The Flags parameter is invalid.
    MemoryAllocationErrors_InvalidVirtualAddress = (1 << 4),  //!< Invalid Virtual Address.
    MemoryAllocationErrors_InvalidPhysicalAddress = (1 << 5), //!< Invalid Physical Address.
    MemoryAllocationErrors_Unknown = (1 << 6),                //!< Unknown Error.
    MemoryAllocationErrors_InvalidParameters = (1 << 7),      //!< Invalid Parameters
    MemoryAllocationErrors_OutOfMemory = (1 << 8),            //!< Out of Memory.
    MemoryAllocationErrors_Deleting = (1 << 9),               //!< Object is being deleted.
} MemoryAllocationErrors;

//!Memory Map operation parameters.
struct MemoryMapParams {
    UID TargetPID;                              //!< The target PID.
    uint64_t PhysicalAddress;                   //!< The physical address to map.
    uint64_t VirtualAddress;                    //!< The virtual address to map to (0 to allocate).
    uint64_t Length;                            //!< The length, in bytes, of the memory to map.
    CachingMode CacheMode;                      //!< The caching mode of the mapping.
    MemoryAllocationType AllocationType;        //!< The allocation type metadata.
    MemoryAllocationFlags AllocationFlags;      //!< The allocation flags.
};


#ifndef _KERNEL_

/**
 * @brief      Map a physical address to a virtual address in any process. R0
 *             process only.
 *
 * @param[in]  pid        The pid
 * @param[in]  phys       The physical address
 * @param      virt       The virtual address (0 to allocate, returned by same
 *                        parameter)
 * @param[in]  len        The length
 * @param[in]  cacheMode  The cache mode
 * @param[in]  type       The type
 * @param[in]  flags      The flags
 *
 * @return     An error code on failure, 0 on success.
 */
static __inline uint64_t
R0_Map(UID pid,
       uint64_t phys,
       uint64_t *virt,
       uint64_t len,
       CachingMode cacheMode,
       MemoryAllocationType type,
       MemoryAllocationFlags flags) {

    if(virt == NULL)
        return -EINVAL;

    struct MemoryMapParams p;

    p.TargetPID = pid;
    p.PhysicalAddress = phys;
    p.VirtualAddress = *virt;
    p.Length = len;
    p.CacheMode = cacheMode;
    p.AllocationType = type;
    p.AllocationFlags = flags;

    *virt = Syscall1(Syscall_R0_Map, (uint64_t)&p);
    return GetErrno();
}

/**
 * @brief      Unmap a virtual address in any process. R0 process only.
 *
 * @param[in]  pid   The pid
 * @param[in]  virt  The virtual address
 * @param[in]  size  The size
 *
 * @return     An error code on failure, 0 on success.
 */
static __inline uint64_t
R0_Unmap(UID pid,
         uint64_t virt,
         uint64_t size) {
    Syscall3(Syscall_R0_Unmap, pid, virt, size);
    return GetErrno();
}

/**
 * @brief      Unmap a virtual address in the current process.
 *
 * @param[in]  virt  The virtual address
 * @param[in]  size  The size
 *
 * @return     An error code on failure, 0 on success.
 */
static __inline uint64_t
Unmap(uint64_t virt,
      uint64_t size) {
    Syscall2(Syscall_Unmap, virt, size);
    return GetErrno();
}

/**
 * @brief      Get the boot information structure. R0 process only.
 *
 * @param      bInfo  The preallocated boot information structure to fill
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
R0_GetBootInfo(CardinalBootInfo *bInfo) {
    Syscall1(Syscall_R0_GetBootInfo, (uint64_t)bInfo);
    return GetErrno();
}

/**
 * @brief      Translate a virtual address to a physical address. R0 and R1 processes
 *             only.
 *
 * @param[in]  pid     The pid
 * @param[in]  v_addr  The virtual address
 * @param[out] p_addr  The physical address
 *
 * @return     The error code on failure, 0 on success.
 */
static __inline uint64_t
R01_GetPhysicalAddress(UID pid, uint64_t v_addr, uint64_t *p_addr) {
    if(p_addr != NULL)
        *p_addr = Syscall2(Syscall_R01_GetPhysicalAddress, pid, v_addr);
    else
        return -EINVAL;

    return GetErrno();
}

/**
 * @brief      Allocate a continuous range of physical pages. R0 process only.
 *
 * @param[in]  cnt     The page count
 * @param[in]  flags   The flags
 * @param[out] p_addr  The physical address
 *
 * @return     The error code on failure, 0 on success.
 */
static __inline uint64_t
R0_AllocatePages(uint64_t cnt, PhysicalMemoryAllocationFlags flags, uint64_t *p_addr) {
    if(p_addr != NULL) {
        *p_addr = Syscall2(Syscall_R0_AllocatePages, cnt, flags);
        return GetErrno();
    }
    return -EINVAL;
}

/**
 * @brief      Free a continuous range of physical pages. R0 process only.
 *
 * @param[in]  p_addr  The p address
 * @param[in]  cnt     The count
 *
 * @return     The error code on failure, 0 on success.
 */
static __inline uint64_t
R0_FreePages(uint64_t p_addr, uint64_t cnt) {
    Syscall2(Syscall_R0_FreePages, p_addr, cnt);
    return GetErrno();
}

#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif