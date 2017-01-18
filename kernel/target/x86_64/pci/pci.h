/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_X86_64_PCI_H_
#define _CARDINAL_X86_64_PCI_H_

#include "types.h"

#define MAX_POSSIBLE_BARS 6

typedef struct PCI_BAR {
    uint64_t value;
    uint32_t offset;
    bool isIOSpace;
} PCI_BAR;

typedef struct {
    uint8_t classCode;
    uint8_t subClassCode;
    uint8_t progIf;

    uint16_t deviceID;
    uint16_t vendorID;

    uint32_t bus;
    uint32_t device;
    uint32_t function;

    uint8_t bar_count;
    PCI_BAR bars[MAX_POSSIBLE_BARS];

    uint8_t headerType;
} PCI_DeviceFuncs;

void
pci_writeDWord(
    uint32_t bus,
    uint32_t device,
    uint32_t function,
    uint32_t offset,
    uint32_t val
);


uint32_t
pci_readDWord(
    uint32_t bus,
    uint32_t device,
    uint32_t function,
    uint32_t offset
);

void
pci_GetPCIClass (
    long classcode,
    char ** base,
    char ** sub,
    char ** prog
);

void
pci_GetPCIDevice(uint16_t venID,
                 uint16_t devID,
                 char **chip_name,
                 char **chip_desc);


void
pci_GetPCIVendor(uint16_t venID,
                 char ** short_name,
                 char ** long_name);

void
pci_Initialize(void);

void
pci_setCommand(uint32_t device_index,
               uint16_t value);


void
pci_regMSIVector(uint32_t device_index,
                 uint8_t vector);

bool
pci_enableMSI(uint32_t device_index);

void
pci_getDeviceInfo(uint32_t pci_index,
                  PCI_DeviceFuncs *p);

uint32_t
pci_getDeviceCount(void);


#endif