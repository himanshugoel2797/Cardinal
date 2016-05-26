#include "controller.h"
#include "uhci/uhci_controller.h"

void
USB_AddController(USBControllerInterface interface,
				  uint64_t bar, 
				  uint32_t (*read)(uint64_t bar, uint32_t offset), 
				  void (*write)(uint64_t bar, uint32_t offset, uint32_t val))
{
	switch(interface)
	{
		case USBControllerInterface_UHCI:
			UHCI_ControllerAdded(bar, read, write);
		break;
		default:
			return;
	}
}