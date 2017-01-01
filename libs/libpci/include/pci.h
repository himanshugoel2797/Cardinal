/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_LIB_PCI_H_
#define _CARDINAL_LIB_PCI_H_

#include <cardinal/cardinal_types.h>

typedef struct {
    uint32_t ClassCode;
    uint32_t SubClassCode;
    uint32_t ProgIF;

    uint32_t HdrType;

    uint32_t DeviceID;
    uint32_t VendorID;

    uint32_t BarCount;

    uint32_t Bus;
    uint32_t Device;
    uint32_t Function;
} PCI_Device;

void
PCI_GetPCIDevice(char *arg_str,
                 PCI_Device *devInfo);

uint64_t
PCI_GetBAR(PCI_Device *device,
           uint32_t bar_index);


uint32_t
PCI_IsIOSpaceBAR(PCI_Device *device,
                 uint32_t bar_index);

void
PCI_EnableBusMaster(PCI_Device *device);

void
PCI_DetectAndAllocateMSI(PCI_Device *device,
                         uint32_t *irq_base,
                         uint32_t *cnt);

uint64_t
PCI_GetBARSize(PCI_Device *device,
               uint32_t bar_index);

#endif