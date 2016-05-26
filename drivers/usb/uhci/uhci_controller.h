#ifndef _CARDINAL_HAL_UHCI_CNTRL_H_
#define _CARDINAL_HAL_UHCI_CNTRL_H_

#include "types.h"

void
UHCI_ControllerAdded(uint64_t bar,
				  	 uint32_t (*read)(uint64_t bar, uint32_t offset), 
				  	 void (*write)(uint64_t bar, uint32_t offset, uint32_t val));


#endif