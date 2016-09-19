#ifndef _CARDINAL_HAL_MEMORY_OPS_H_
#define _CARDINAL_HAL_MEMORY_OPS_H_

#include "types.h"
#include "synchronization.h"
#include "list.h"
#include "libs/libCardinal/include/memory.h"

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
#define PAGE_SIZE KiB(4)

#define THREAD_LOCAL_STORAGE_SIZE KiB(32)


typedef struct MemoryAllocationsMap {
    CachingMode     CacheMode;
    uint64_t    VirtualAddress;
    uint64_t    PhysicalAddress;
    uint64_t    Length;
    MemoryAllocationFlags Flags;
    MemoryAllocationType  AllocationType;
    void*    AdditionalData;

    struct MemoryAllocationsMap *next;
    struct MemoryAllocationsMap *prev;
} MemoryAllocationsMap;

typedef struct ForkedMemoryData {
    CachingMode CacheMode;
    uint64_t VirtualAddress;
    uint64_t PhysicalAddress;
    uint64_t Length;
    uint32_t NetReferenceCount;
    MemoryAllocationFlags Flags;
    MemoryAllocationType AllocationType;
    Spinlock Lock;
} ForkedMemoryData;

typedef struct SharedMemoryKey {
    uint64_t Key;
    MemoryAllocationFlags Permissions;
} SharedMemoryKey;

typedef struct SharedMemoryData {
    List *Keys;
    MemoryAllocationFlags Permissions;
    UID MasterProcess;
    uint64_t VirtualAddress;
    uint64_t PhysicalAddress;
    uint64_t Length;
    uint32_t MasterKey;
    uint32_t NetReferenceCount;
    Spinlock Lock;
} SharedMemoryData;

typedef struct ManagedPageTable {
    UID                   PageTable;
    MemoryAllocationsMap  *AllocationMap;

    uint32_t              reference_count;
    Spinlock              lock;
} ManagedPageTable;

void*
GetVirtualAddress(CachingMode   c,
                  void    *physicalAddress);

void*
GetPhysicalAddress(void *virtualAddress);

void*
GetPhysicalAddressPageTable(ManagedPageTable   *src,
                            void 	             *virtualAddress);

MemoryAllocationErrors
CreateVirtualMemoryInstance(ManagedPageTable *inst);

void
FreeVirtualMemoryInstance(ManagedPageTable *inst);

ManagedPageTable*
SetActiveVirtualMemoryInstance(ManagedPageTable* inst);

ManagedPageTable*
GetActiveVirtualMemoryInstance(void);

MemoryAllocationErrors
ForkTable(ManagedPageTable *src,
          ManagedPageTable *dst);

MemoryAllocationErrors
MapPage(ManagedPageTable *pageTable,
        uint64_t 		physicalAddress,
        uint64_t 		virtualAddress,
        size_t			size,
        CachingMode 		cacheMode,
        MemoryAllocationType 	allocType,
        MemoryAllocationFlags 	flags);


MemoryAllocationErrors
ChangePageFlags(ManagedPageTable *pageTable,
                uint64_t        virtualAddress,
                CachingMode         cacheMode,
                MemoryAllocationType    allocType,
                MemoryAllocationFlags   flags);

MemoryAllocationErrors
UnmapPage(ManagedPageTable* 			pageTable,
          uint64_t 		virtualAddress,
          size_t 		size);

uint64_t
GetMemoryAllocationTypeTop(MemoryAllocationType allocType,
                           MemoryAllocationFlags sec_perms);

uint64_t
GetMemoryAllocationTypeBase(MemoryAllocationType allocType,
                            MemoryAllocationFlags sec_perms);

MemoryAllocationErrors
FindFreeVirtualAddress(ManagedPageTable* 			pageTable,
                       uint64_t			*virtualAddress,
                       size_t 			size,
                       MemoryAllocationType 	allocType,
                       MemoryAllocationFlags 	flags);

void
HandlePageFault(uint64_t virtualAddress,
                uint64_t instruction_pointer,
                MemoryAllocationFlags error);

void
PerformTLBShootdown(void);


uint64_t
SetupTemporaryWriteMap(ManagedPageTable *pageTable,
                       uint64_t addr,
                       size_t size);

void
UninstallTemporaryWriteMap(uint64_t loc,
                           size_t size);


void
CheckAddressPermissions(ManagedPageTable *pageTable,
                        uint64_t addr,
                        CachingMode *cacheMode,
                        MemoryAllocationFlags *flags);

void
WriteValueAtAddress64(ManagedPageTable *pageTable,
                      uint64_t *addr,
                      uint64_t val);

void
WriteValueAtAddress32(ManagedPageTable *pageTable,
                      uint32_t *addr,
                      uint32_t val);

uint64_t
AllocatePhysicalPage(void);

void
FreePhysicalPage(uint64_t ptr);

uint64_t
AllocatePhysicalPageCont(int pageCount);

void
FreePhysicalPageCont(uint64_t ptr,
                     int pageCount);

void*
AllocateAPLSMemory(uint64_t size);

int
GetCoreCount(void);

void
HaltProcessor(void);

uint64_t
ManageSharedMemoryKey(size_t size, uint64_t flags, uint64_t key);

uint64_t
SharedMemoryKeyAction(uint64_t key, uint64_t flags);

void
WipeMemoryTypeFromTable(ManagedPageTable *pageTable,
                        MemoryAllocationType type);

#endif
