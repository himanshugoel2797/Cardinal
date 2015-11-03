#ifndef _NETWORK_RTL_8139_DRIV_H_
#define _NETWORK_RTL_8139_DRIV_H_

#include "types.h"
#include "../network.h"

bool
RTL8139_Detect(uint32_t index);

uint32_t
RTL8139_Initialize(uint32_t deviceIndex);

uint32_t
RTL8139_Reset(void);

void
RTL8139_Transmit(void *packet,
                 uint16_t len);

#endif