/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "uhci.h"
#include "utils/native.h"

bool
UHCI_DetectController(PCI_DeviceFuncs *device) {
    return (device->classCode == 0x0C && device->subClassCode == 0x03 && device->progIf == 0x00);
}

static uint32_t
UHCI_Read(uint64_t bar,
          uint32_t offset) {
    if(bar & 1) {
        bar &= 0xFFFE;
        return inl((uint16_t)(bar + offset));
    } else {
        uint32_t *b = (uint32_t*)(bar + offset);
        return *b;
    }
}

static void
UHCI_Write(uint64_t bar,
           uint32_t offset,
           uint32_t value) {
    if(bar & 1) {
        bar &= 0xFFFE;
        outl((uint16_t)(bar + offset), value);
    } else {
        uint32_t *b = (uint32_t*)(bar + offset);
        *b = value;
    }
}


bool
UHCI_InitializeController(PCI_DeviceFuncs *device) {
    //Setup the host controller and pass the base address and accessors to the usb stack
    uint64_t bar = 0;
    for(int i = 0; i < device->bar_count; i++) {
        if(device->bars[i].offset == 0x20) {
            bar = device->bars[i].value;
            bar |= (device->bars[i].isIOSpace & 1);
            break;
        }
    }

    if(bar == 0)return FALSE;

    //The bar has been retrieved, pass on this data along with IO handlers to the uhci stack
    USB_AddController(USBControllerInterface_UHCI,
                      bar,
                      UHCI_Read,
                      UHCI_Write);

    return TRUE;
}

void
UHCI_ActivationStateChange(DeviceActivationState state) {
    state = DeviceActivationState_Disabled;
}