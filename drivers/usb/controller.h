#ifndef _CARDINAL_USB_CONTROLLER_H_
#define _CARDINAL_USB_CONTROLLER_H_

#include "types.h"

void
USB_AddController(uint64_t bar, 
				  uint32_t (*read)(uint64_t bar, uint32_t offset), 
				  void (*write)(uint64_t bar, uint32_t offset, uint32_t val));

#endif