#ifndef _CARDINAL_USB_CONTROLLER_H_
#define _CARDINAL_USB_CONTROLLER_H_

#include "types.h"

typedef enum USBControllerInterface{
	USBControllerInterface_UHCI,
	USBControllerInterface_OHCI,
	USBControllerInterface_EHCI,
	USBControllerInterface_xHCI
}USBControllerInterface;

void
USB_AddController(USBControllerInterface interface,
				  uint64_t bar, 
				  uint32_t (*read)(uint64_t bar, uint32_t offset), 
				  void (*write)(uint64_t bar, uint32_t offset, uint32_t val));

#endif