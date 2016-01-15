#include "memory.h"
#include "virt_mem_manager/virt_mem_manager.h"

void*
GetVirtualAddress(void *physicalAddress)
{
    return VirtMemMan_GetVirtualAddress(physicalAddress);
}

void*
GetPhysicalAddress(void *virtualAddress)
{
  return VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), virtualAddress);
}
