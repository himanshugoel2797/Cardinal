/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/cardinal_types.h>
#include <cardinal/driver_utils.h>

#include "pci.h"

uint32_t
PCI_ReadDWord(uint32_t bus,
              uint32_t device,
              uint32_t function,
              uint32_t offset) {
    outl(PCI_ADDR, 0x80000000 | bus << 16 | device << 11 | function <<  8 | (offset & 0xfc));
    return inl(PCI_DATA);
}

void
PCI_WriteDWord(uint32_t bus,
               uint32_t device,
               uint32_t function,
               uint32_t offset,
               uint32_t val) {
    outl(PCI_ADDR, 0x80000000 | bus << 16 | device << 11 | function <<  8 | (offset & 0xfc));
    outl(PCI_DATA, val);
}

void
PCI_GetNextDevice(uint32_t *bus,
                  uint32_t *device) {

    uint32_t b = *bus;
    uint32_t d = *device;

    for(; b < 256; b++)
        for(; d < 32; d++) {
            uint32_t v_id = PCI_ReadDWord(b, d, 0, 0);

            if(v_id != 0xFFFFFFFF) {
                *bus = b;
                *device = d;
                return;
            }
        }

    *bus = -1;
    *device = -1;
}

uint32_t
PCI_GetFuncCount(uint32_t bus,
                 uint32_t device) {
    uint32_t hdrType = PCI_ReadDWord(bus, device, 0, 0x0C);

    if((hdrType >> 23) & 1)
        return 8;

    return 1;
}

void
PCI_GetPCIDevice(uint32_t bus,
                 uint32_t device,
                 uint32_t function,
                 PCI_Device *devInfo) {

    devInfo->ClassCode = PCI_ReadDWord(bus, device, function, 8) >> 24;
    devInfo->SubClassCode = (PCI_ReadDWord(bus, device, function, 8) >> 16) & 0xFF;
    devInfo->ProgIF = (PCI_ReadDWord(bus, device, function, 8) >> 8) & 0xFF;

    devInfo->HdrType = (PCI_ReadDWord(bus, device, function, 0x0C) >> 16) & 0xFF;

    devInfo->DeviceID = (uint16_t)(PCI_ReadDWord(bus, device, function, 0) >> 16);
    devInfo->VendorID = (uint16_t)PCI_ReadDWord(bus, device, function, 0);

    devInfo->Bus = bus;
    devInfo->Device = device;
    devInfo->Function = function;

    if(devInfo->HdrType == 0)
        devInfo->BarCount = 6;
    else
        devInfo->BarCount = 2;
}

uint64_t
PCI_GetBAR(PCI_Device *device,
           uint32_t bar_index) {
    uint32_t l_word = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4));

    if(l_word & 1) {
        return l_word & ~1;
    } else if(((l_word >> 1) & 3) == 0) {
        return l_word & ~0xF;
    } else if(((l_word >> 1) & 3) == 2) {
        return (l_word & ~0xF) | (uint64_t)(PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index + 1) * 4)) << 32;
    }

    return l_word;
}

uint32_t
PCI_IsIOSpaceBAR(PCI_Device *device,
                 uint32_t bar_index) {
    uint32_t l_word = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4));

    return l_word & 1;
}

uint64_t
PCI_GetBARSize(PCI_Device *device,
               uint32_t bar_index) {
    uint64_t bar_val = PCI_GetBAR(device, bar_index);

    PCI_WriteDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4), 0xFFFFFFFF);
    uint32_t val = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4));

    PCI_WriteDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4), bar_val);

    return ~val + 1;
}

int
PCI_AllocateMSI(PCI_Device *device) {

    //Traverse the capabilities list
    uint32_t status = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x04);
    status &= ~(1 << 10); //Disable legacy interrupts
    PCI_WriteDWord(device->Bus, device->Device, device->Function, 0x04, status);

    //Check the Status register, bit 4 to see if the capabilities list is present
    if((status >> 4) & 1) {
        uint8_t cap_list_offset = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x34) & 0xFC;

        uint32_t msi_ctrl_off = cap_list_offset << 8;
        uint32_t msi_ctrl_off_nx = msi_ctrl_off & 0xFFFF;

        while( (msi_ctrl_off_nx & 0xFF) != 0x05) {
            msi_ctrl_off = msi_ctrl_off_nx;
            if(msi_ctrl_off == 0)return -1;

            msi_ctrl_off_nx = PCI_ReadDWord(
                                  device->Bus,
                                  device->Device,
                                  device->Function,
                                  msi_ctrl_off >> 8) & 0xFFFF;
        }

        msi_ctrl_off = msi_ctrl_off >> 8;

        uint32_t reg = PCI_ReadDWord(
                           device->Bus,
                           device->Device,
                           device->Function,
                           msi_ctrl_off);

        uint32_t msi_64b_cap = (reg >> 16) >> 7;
        reg |= (1 << 16);        //Enable MSI


        PCI_WriteDWord(
            device->Bus,
            device->Device,
            device->Function,
            msi_ctrl_off,
            reg);

        uint32_t data_reg = msi_ctrl_off + 8;
        if(msi_64b_cap)
            data_reg += 4;

        reg = PCI_ReadDWord(
                  device->Bus,
                  device->Device,
                  device->Function,
                  msi_ctrl_off + 4);
        reg = 0xFEE00000;   //Set the MSI Address as per x86 specs
        //TODO get the destination ID by adding a new arch specific property syscall.
        //reg |= APIC_GetID() << 12;  //Set the APIC address

        PCI_WriteDWord(
            device->Bus,
            device->Device,
            device->Function,
            msi_ctrl_off + 4,
            reg);


        reg = PCI_ReadDWord(
                  device->Bus,
                  device->Device,
                  device->Function,
                  data_reg);
        reg &= ~0xFF;
        //TODO allocate irqs
        //reg |=

        PCI_WriteDWord(
            device->Bus,
            device->Device,
            device->Function,
            data_reg,
            reg);

        return reg & 0xFF;

    }

    return -1;
}