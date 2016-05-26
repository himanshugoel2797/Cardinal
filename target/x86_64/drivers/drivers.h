#ifndef _X86_64_CARDINAL_DRIVERS_H_
#define _X86_64_CARDINAL_DRIVERS_H_

#include "managers.h"
#include "pci/pci.h"

typedef struct Cardinal_Driver{
	char name[MAX_DEVICE_NAME_LEN];
	char bus_name[MAX_BUS_NAME_LEN];
	DeviceType dev_type;
	DeviceActivationStateChangeHandler handler;
	bool (*detector)(PCI_DeviceFuncs *device);
	bool (*init)(PCI_DeviceFuncs *device);
}Cardinal_Driver;

#include "uhci/uhci.h"

const Cardinal_Driver drivers[] = {
{"UHCI", "PCI", DeviceType_SerialBus, UHCI_ActivationStateChange, UHCI_DetectController, UHCI_InitializeController},
{"","", DeviceType_Unknown, NULL, NULL, NULL}
};
#endif