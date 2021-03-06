/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _VIRT_MEM_MAN_X86_64_H_
#define _VIRT_MEM_MAN_X86_64_H_

#include "types.h"
#include "memory.h"

#define APLS_SIZE THREAD_LOCAL_STORAGE_SIZE
#define SHMEM_VADDR_BASE (30ULL << 39)

typedef uint64_t* PML_Instance;

typedef enum {
    MEM_TYPE_UC = 2,	//Uncached
    MEM_TYPE_WB = 0,	//Writeback
    MEM_TYPE_WT = 1,	//Write through
    MEM_TYPE_WC = 3	//Write complete
} MEM_TYPES;		//Memory caching types

typedef enum {
    MEM_WRITE = 2,	//Writable
    MEM_READ = 4,	//Readable
    MEM_EXEC = 1,	//Executable
    MEM_PRESENT = 8, //Present
} MEM_ACCESS_PERMS;	//Memory Permissions

typedef enum {
    MEM_KERNEL = 0,	//Kernel access only
    MEM_USER = 1	//User access only
} MEM_SECURITY_PERMS;	//Memory access permissions

void
VirtMemMan_Initialize(void);

void
VirtMemMan_InitializeBootstrap(void);

PML_Instance
VirtMemMan_CreateInstance(void);

PML_Instance
VirtMemMan_SetCurrent(PML_Instance instance);

PML_Instance
VirtMemMan_GetCurrent(void);

void
VirtMemMan_Unmap(    PML_Instance     inst,
                     uint64_t         virt_addr,
                     uint64_t         size);

void
VirtMemMan_MapHPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms);

void
VirtMemMan_MapLPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms);

void
VirtMemMan_MapSPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms);

void
VirtMemMan_Map(PML_Instance       inst,
               uint64_t           virt_addr,
               uint64_t           phys_addr,
               uint64_t           size,
               bool               present,
               MEM_TYPES          cache,
               MEM_ACCESS_PERMS   access_perm,
               MEM_SECURITY_PERMS sec_perms);

void*
VirtMemMan_GetPhysicalAddress(PML_Instance  inst,
                              void         *addr);

void*
VirtMemMan_GetVirtualAddress(CachingMode c,
                             void 	 *addr);

uint64_t
VirtMemMan_GetAllocTypeBase(MemoryAllocationType allocType,
                            MEM_SECURITY_PERMS sec_perms);

uint64_t
VirtMemMan_GetAllocTypeTop(MemoryAllocationType allocType,
                           MEM_SECURITY_PERMS sec_perms);

uint64_t
VirtMemMan_FindFreeAddress(PML_Instance       inst,
                           uint64_t           *addr,
                           uint64_t           size,
                           MemoryAllocationType allocType,
                           MEM_SECURITY_PERMS sec_perms);

void
VirtMemMan_GetAddressPermissions(PML_Instance inst,
                                 uint64_t addr,
                                 MEM_TYPES *cacheType,
                                 MEM_ACCESS_PERMS *access_perm,
                                 MEM_SECURITY_PERMS *security_perm);

void*
VirtMemMan_AllocCoreLocalData(uint64_t size);

void
VirtMemMan_FreePageTable(PML_Instance inst);

void
VirtMemMan_HandlePageFault(uint32_t int_no,
                           uint32_t err_code);

uint64_t
VirtMemMan_GetPageSize(PML_Instance inst,
                       uint64_t vaddr);

#endif
