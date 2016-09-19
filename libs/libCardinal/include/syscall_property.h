#ifndef _LIB_CARDINAL_PROPERTY_H_
#define _LIB_CARDINAL_PROPERTY_H_

#include <stdint.h>
#include <stddef.h>

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
#ifdef __x86_64__
    CardinalProperty_ArchPrctl,
    CardinalProperty_IOPL
#endif
} CardinalProperties;

#endif
