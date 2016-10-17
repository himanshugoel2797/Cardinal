#ifndef _CARDINAL_HAL_MEMORY_OPS_H_
#define _CARDINAL_HAL_MEMORY_OPS_H_

#include "types.h"
#include "synchronization.h"
#include "list.h"
#include "libs/libCardinal/include/memory.h"

/**
 * \defgroup memory_hal Memory Management
 * @{
 */

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
/**
 * The default page size.
 */
#define PAGE_SIZE KiB(4)

/**
 * The size of the TLS.
 */
#define THREAD_LOCAL_STORAGE_SIZE KiB(32)

#ifdef __x86_64__
#define CORE_LOCAL __attribute__((address_space(256)))
#endif

/**
 * Describes a memory allocation in an address space.
 */
typedef struct MemoryAllocationsMap {
    CachingMode     CacheMode;            //!< The CachingMode used for this allocation.
    uint64_t    VirtualAddress;           //!< The virtual address of this allocation.
    uint64_t    PhysicalAddress;          //!< The physical address of this allocation.
    uint64_t    Length;                   //!< The length, in bytes, of this allocation
    MemoryAllocationFlags Flags;          //!< The MemoryAllocationFlags of this allocation.
    MemoryAllocationType  AllocationType; //!< The MemoryAllocationType of this allocation.

    struct MemoryAllocationsMap *next;    //!< The next allocation in the list.
    struct MemoryAllocationsMap *prev;    //!< The previous allocation in the list.
} MemoryAllocationsMap;

/**
 * Describes a hardware independent page table
 */
typedef struct ManagedPageTable {
    UID                   PageTable;      //!< The hardware dependent page table
    MemoryAllocationsMap  *AllocationMap; //!< The allocation data of the page table

    uint32_t              reference_count;
    Spinlock              lock;
} ManagedPageTable;

/**
 * @brief      Gets the virtual address.
 *
 * @param[in]  c                The caching mode to translate from
 * @param[in]  physicalAddress  The physical address
 *
 * @return     The virtual address.
 */
void*
GetVirtualAddress(CachingMode   c,
                  void    *physicalAddress);

/**
 * @brief      Gets the physical address.
 *
 * @param[in]  virtualAddress  The virtual address
 *
 * @return     The physical address.
 */
void*
GetPhysicalAddress(void *virtualAddress);

/**
 * @brief      Gets the physical address from a specific page table.
 *
 * @param[in]  src             The page table
 * @param[in]  virtualAddress  The virtual address
 *
 * @return     The physical address from the page table.
 */
void*
GetPhysicalAddressPageTable(ManagedPageTable   *src,
                            void 	             *virtualAddress);

/**
 * @brief      Creates a virtual memory instance.
 *
 * @param      inst  The instance to initialize
 *
 * @return     Error code on failure, MemoryAllocationErrors_None on success.
 */
MemoryAllocationErrors
CreateVirtualMemoryInstance(ManagedPageTable *inst);

/**
 * @brief      Free a virtual memory instance.
 *
 * @param      inst  The instance
 */
void
FreeVirtualMemoryInstance(ManagedPageTable *inst);

/**
 * @brief      Sets the active virtual memory instance.
 *
 * @param      inst  The instance
 *
 * @return     Return the previously active virtual memory instance.
 */
ManagedPageTable*
SetActiveVirtualMemoryInstance(ManagedPageTable* inst);

/**
 * @brief      Gets the active virtual memory instance.
 *
 * @return     The active virtual memory instance.
 */
ManagedPageTable*
GetActiveVirtualMemoryInstance(void);

/**
 * @brief      Map a page into the page table.
 *
 * @param[in]  pageTable        The page table
 * @param[in]  physicalAddress  The physical address
 * @param[in]  virtualAddress   The virtual address
 * @param[in]  size             The size
 * @param[in]  cacheMode        The cache mode
 * @param[in]  allocType        The allocate type
 * @param[in]  flags            The flags
 *
 * @return     Error code on failure, MemoryAllocationErrors_None on success.
 */
MemoryAllocationErrors
MapPage(ManagedPageTable *pageTable,
        uint64_t 		physicalAddress,
        uint64_t 		virtualAddress,
        size_t			size,
        CachingMode 		cacheMode,
        MemoryAllocationType 	allocType,
        MemoryAllocationFlags 	flags);

/**
 * @brief      Change the flags on an existing page mapping.
 *
 * @param[in]  pageTable       The page table
 * @param[in]  virtualAddress  The virtual address
 * @param[in]  cacheMode       The cache mode
 * @param[in]  allocType       The allocate type
 * @param[in]  flags           The flags
 *
 * @return     Error code on failure, MemoryAllocationErrors_None on success.
 */
MemoryAllocationErrors
ChangePageFlags(ManagedPageTable *pageTable,
                uint64_t        virtualAddress,
                CachingMode         cacheMode,
                MemoryAllocationType    allocType,
                MemoryAllocationFlags   flags);

/**
 * @brief      Unmap an existing page mapping.
 *
 * @param[in]  pageTable       The page table
 * @param[in]  virtualAddress  The virtual address
 * @param[in]  size            The size
 *
 * @return     Error code on failure, MemoryAllocationErrors_None on success.
 */
MemoryAllocationErrors
UnmapPage(ManagedPageTable* 			pageTable,
          uint64_t 		virtualAddress,
          size_t 		size);

/**
 * @brief      Gets the upper extent of the given memory allocation type in virtual memory.
 *
 * @param[in]  allocType  The allocate type
 * @param[in]  sec_perms  The security permissions
 *
 * @return     The memory allocation type top address.
 */
uint64_t
GetMemoryAllocationTypeTop(MemoryAllocationType allocType,
                           MemoryAllocationFlags sec_perms);

/**
 * @brief      Gets the low extent of the given memory allocation type in virtual memory.
 *
 * @param[in]  allocType  The allocate type
 * @param[in]  sec_perms  The security permissions
 *
 * @return     The memory allocation type base address.
 */
uint64_t
GetMemoryAllocationTypeBase(MemoryAllocationType allocType,
                            MemoryAllocationFlags sec_perms);

/**
 * @brief      Find a free virtual address
 *
 * @param[in]  pageTable       The page table
 * @param[in]  virtualAddress  The virtual address
 * @param[in]  size            The size
 * @param[in]  allocType       The allocate type
 * @param[in]  flags           The flags
 *
 * @return     Error code on failure, MemoryAllocationErrors_None on success.
 */
MemoryAllocationErrors
FindFreeVirtualAddress(ManagedPageTable* 			pageTable,
                       uint64_t			*virtualAddress,
                       size_t 			size,
                       MemoryAllocationType 	allocType,
                       MemoryAllocationFlags 	flags);

/**
 * @brief      Handle a page fault.
 *
 * @param[in]  virtualAddress       The virtual address
 * @param[in]  instruction_pointer  The instruction pointer
 * @param[in]  error                The error
 */
void
HandlePageFault(uint64_t virtualAddress,
                uint64_t instruction_pointer,
                MemoryAllocationFlags error);

/**
 * @brief      Perform a TLB shootdown.
 */
void
PerformTLBShootdown(void);

/**
 * @brief      Setup a temporary writeable mapping of the virtual address in the
 *             specified page table into the current space.
 *
 * @param[in]  pageTable  The page table
 * @param[in]  addr       The address
 * @param[in]  size       The size
 *
 * @return     The virtual address of the mapping in the current address space.
 */
uint64_t
SetupTemporaryWriteMap(ManagedPageTable *pageTable,
                       uint64_t addr,
                       size_t size);

/**
 * @brief      Uninstall the temporary write mapping setup by
 *             SetupTemporaryWriteMap()
 *
 * @param[in]  loc   The location
 * @param[in]  size  The size
 */
void
UninstallTemporaryWriteMap(uint64_t loc,
                           size_t size);

/**
 * @brief      Retrieve the access permissions on a mapping.
 *
 * @param[in]  pageTable  The page table
 * @param[in]  addr       The address
 * @param[out] cacheMode  The cache mode
 * @param[out] flags      The flags
 */
void
CheckAddressPermissions(ManagedPageTable *pageTable,
                        uint64_t addr,
                        CachingMode *cacheMode,
                        MemoryAllocationFlags *flags);

/**
 * @brief      Writes a 64-bit value at the specified address in the specified
 *             page table.
 *
 * @param[in]  pageTable  The page table
 * @param[in]  addr       The address
 * @param[in]  val        The value
 */
void
WriteValueAtAddress64(ManagedPageTable *pageTable,
                      uint64_t *addr,
                      uint64_t val);

/**
 * @brief      Writes a 32-bit value at the specified address in the specified
 *             page table.
 *
 * @param[in]  pageTable  The page table
 * @param[in]  addr       The address
 * @param[in]  val        The value
 */
void
WriteValueAtAddress32(ManagedPageTable *pageTable,
                      uint32_t *addr,
                      uint32_t val);

/**
 * @brief      Allocate a physical page.
 *
 * @return     0 on error, The physical address of the allocation on success.
 */
uint64_t
AllocatePhysicalPage(void);

/**
 * @brief      Free a previously allocated page.
 *
 * @param[in]  ptr   The physical address of the page to free.
 */
void
FreePhysicalPage(uint64_t ptr);

/**
 * @brief      Allocate a continuous set of physical pages.
 *
 * @param[in]  pageCount  The page count
 *
 * @return     0 on error, The physical address of the allocation on success.
 */
uint64_t
AllocatePhysicalPageCont(int pageCount);

/**
 * @brief      Free a continuous set of previously allocated pages.
 *
 * @param[in]  ptr        The physical address of the page to start freeing from
 * @param[in]  pageCount  The page count
 */
void
FreePhysicalPageCont(uint64_t ptr,
                     int pageCount);

/**
 * @brief      Allocate Application Processor Local Storage.
 *
 * @param[in]  size  The size of the allocation.
 *
 * @return     NULL on error, The virtual address of the allocation on success.
 */
void*
AllocateAPLSMemory(uint64_t size);

/**
 * @brief      Gets the core count.
 *
 * @return     The core count.
 */
int
GetCoreCount(void);

/**
 * @brief      Halt the processor.
 */
void
HaltProcessor(void);

/**
 * @brief      Wipe all allocations of the specified type from the specified
 *             page table.
 *
 * @param[in]  pageTable  The page table
 * @param[in]  type       The type
 */
void
WipeMemoryTypeFromTable(ManagedPageTable *pageTable,
                        MemoryAllocationType type);

/**@}*/

#endif
