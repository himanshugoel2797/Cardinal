#include "memory.h"
#include "virt_mem_manager/virt_mem_manager.h"

void*
GetVirtualAddress(CachingMode c,
                  void *physicalAddress)
{
    return VirtMemMan_GetVirtualAddress(c, physicalAddress);
}

void*
GetPhysicalAddress(void *virtualAddress)
{
    return VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), virtualAddress);
}
