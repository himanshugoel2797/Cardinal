#ifndef _X86_64_CARDINAL_UHCI_H_
#define _X86_64_CARDINAL_UHCI_H_

#include "types.h"
#include "pci/pci.h"
#include "managers.h"
#include "drivers/usb/usb.h"

bool
UHCI_DetectController(PCI_DeviceFuncs *device);

bool
UHCI_InitializeController(PCI_DeviceFuncs *device);

void
UHCI_ActivationStateChange(DeviceActivationState state);

#endif