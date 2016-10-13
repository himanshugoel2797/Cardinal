#ifndef _LIB_CARDINAL_PROPERTY_H_
#define _LIB_CARDINAL_PROPERTY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "syscall_property.h"
#include "syscall.h"

typedef enum {
    CardinalProperty_TID,
    CardinalProperty_PID,
    CardinalProperty_PLS, /*Process Local Storage*/
    CardinalProperty_SetTidAddress,
    CardinalProperty_Time,
    CardinalProperty_SpecialDestination,
    CardinalProperty_Exit,
    CardinalProperty_GroupID,
    CardinalProperty_R0_BootInfo,
    CardinalProperty_R0_PhysicalAddress,
#ifdef __x86_64__
    CardinalProperty_ArchPrctl,
    CardinalProperty_IOPL
#endif
} CardinalProperties;

#ifndef _KERNEL_

static __inline uint64_t
GetProperty(CardinalProperties prop, uint64_t type) {
    return Syscall2(Syscall_GetProperty, prop, type);
}

static __inline bool
SetProperty(CardinalProperties prop, uint64_t type, uint64_t val) {
    return Syscall3(Syscall_SetProperty, prop, type, val);
}

#endif

#endif
