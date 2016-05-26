#include "uhci_controller.h"

void
UHCI_ControllerAdded(uint64_t bar,
				  	 uint32_t (*read)(uint64_t bar, uint32_t offset), 
				  	 void (*write)(uint64_t bar, uint32_t offset, uint32_t val))
{
	bar = 0;
	read = NULL;
	write = NULL;
}