#ifndef _CARDINAL_HAL_MEMORY_OPS_H_
#define _CARDINAL_HAL_MEMORY_OPS_H_

#include "types.h"

void*
GetVirtualAddress(void *physicalAddress);

void*
GetPhysicalAddress(void *virtualAddress);

#endif
