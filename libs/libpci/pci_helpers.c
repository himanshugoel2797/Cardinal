#include <cardinal/driver_utils.h>
#include <stdio.h>
#include "pci.h"


#define PCI_ADDR 0xCF8
#define PCI_DATA 0xCFC


static uint32_t
PCI_ReadDWord(uint32_t bus,
              uint32_t device,
              uint32_t function,
              uint32_t offset)
{
    outl(PCI_ADDR, 0x80000000 | bus << 16 | device << 11 | function <<  8 | (offset & 0xfc));
    return inl(PCI_DATA);
}

static void
PCI_WriteDWord(uint32_t bus,
               uint32_t device,
               uint32_t function,
               uint32_t offset,
               uint32_t val)
{
    outl(PCI_ADDR, 0x80000000 | bus << 16 | device << 11 | function <<  8 | (offset & 0xfc));
    outl(PCI_DATA, val);
}


void
PCI_GetPCIDevice(char *str,
                 PCI_Device *devInfo) 
{

	sscanf(str, "B:%d D:%d F:%d DID:%x VID:%x", &devInfo->Bus, 
												&devInfo->Device, 
												&devInfo->Function, 
												&devInfo->DeviceID, 
												&devInfo->VendorID);

	uint32_t bus = devInfo->Bus;
	uint32_t device = devInfo->Device;
	uint32_t function = devInfo->Function;

    devInfo->ClassCode = PCI_ReadDWord(bus, device, function, 8) >> 24;
    devInfo->SubClassCode = (PCI_ReadDWord(bus, device, function, 8) >> 16) & 0xFF;
    devInfo->ProgIF = (PCI_ReadDWord(bus, device, function, 8) >> 8) & 0xFF;

    devInfo->HdrType = (PCI_ReadDWord(bus, device, function, 0x0C) >> 16) & 0xFF;

    devInfo->DeviceID = (uint16_t)(PCI_ReadDWord(bus, device, function, 0) >> 16);
    devInfo->VendorID = (uint16_t)PCI_ReadDWord(bus, device, function, 0);

    if(devInfo->HdrType == 0)
        devInfo->BarCount = 6;
    else
        devInfo->BarCount = 2;
}

uint64_t
PCI_GetBAR(PCI_Device *device,
           uint32_t bar_index)
{
    uint32_t l_word = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4));

    if(l_word & 1) {
        return l_word & ~1;
    }else if(((l_word >> 1) & 3) == 0) {
        return l_word & ~0xF;
    }else if(((l_word >> 1) & 3) == 2) {
        return (l_word & ~0xF) | (uint64_t)(PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index + 1) * 4)) << 32;
    }

    return l_word;
}

uint32_t
PCI_IsIOSpaceBAR(PCI_Device *device,
                 uint32_t bar_index)
{
    uint32_t l_word = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4));

    return l_word & 1;
}


uint64_t
PCI_GetBARSize(PCI_Device *device,
               uint32_t bar_index)
{
    uint64_t bar_val = PCI_GetBAR(device, bar_index);

    PCI_WriteDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4), 0xFFFFFFFF);
    uint32_t val = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4));

    PCI_WriteDWord(device->Bus, device->Device, device->Function, 0x10 + (bar_index * 4), bar_val);

    return ~val + 1;
}

void
PCI_EnableBusMaster(PCI_Device *device)
{
    uint64_t cmd_val = PCI_ReadDWord(device->Bus, device->Device, device->Function, 0x4);
    cmd_val |= 4;
    PCI_WriteDWord(device->Bus, device->Device, device->Function, 0x4, cmd_val);
}