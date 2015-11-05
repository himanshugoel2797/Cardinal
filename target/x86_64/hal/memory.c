#include "memory.h"
#include "virt_mem_manager/virt_mem_manager.h"

void*
GetVirtualAddress(void *physicalAddress)
{
    return (void*)((uint64_t)physicalAddress + 0xfffffffe00000000ull);
}

void*
GetPhysicalAddress(void *virtualAddress)
{
  return VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), virtualAddress);
}
