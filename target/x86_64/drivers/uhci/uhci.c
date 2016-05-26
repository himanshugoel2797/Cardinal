#include "uhci.h"

bool 
UHCI_DetectController(PCI_DeviceFuncs *device)
{
	return (device->classCode == 0x0C && device->subClassCode == 0x03 && device->progIf == 0x00);
}

bool
UHCI_InitializeController(PCI_DeviceFuncs *device)
{
	//Setup the host controller and pass the base address and accessors to the usb stack
	uint64_t bar = 0;
	for(int i = 0; i < device->bar_count; i++)
	{
		if(device->bars[i].offset == 0x20)
		{
			bar = device->bars[i].value;
			bar |= (device->bars[i].isIOSpace & 1);
		}
	}

	if(bar == 0)return FALSE;

	//The bar has been retrieved, pass on this data along with IO handlers to the uhci stack

	return TRUE;
}

void 
UHCI_ActivationStateChange(DeviceActivationState state)
{
	state = DeviceActivationState_Disabled;
}