#ifndef _LIB_CARDINAL_PROPERTY_H_
#define _LIB_CARDINAL_PROPERTY_H_

#include <stdint.h>
#include <stddef.h>

typedef enum {
    CardinalProperty_SetTidAddress,
    CardinalProperty_RegisterIPCMessageHandler,
#ifdef x86_64
    CardinalProperty_ArchPrctl,
    CardinalProperty_IOPL
#endif
} CardinalProperties;

#endif
