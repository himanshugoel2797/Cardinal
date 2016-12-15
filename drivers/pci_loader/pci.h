#ifndef _PCI_LOADER_PCI_H_
#define _PCI_LOADER_PCI_H_

#include <cardinal/cardinal_types.h>

#define PCI_ADDR 0xCF8
#define PCI_DATA 0xCFC

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
PCI_GetNextDevice(uint32_t *bus,
                  uint32_t *device);


uint32_t
PCI_GetFuncCount(uint32_t bus,
                 uint32_t device);


void
PCI_GetPCIDevice(uint32_t bus,
                 uint32_t device,
                 uint32_t function,
                 PCI_Device *devInfo);


uint64_t
PCI_GetBAR(PCI_Device *device,
           uint32_t bar_index);


uint32_t
PCI_IsIOSpaceBAR(PCI_Device *device,
                 uint32_t bar_index);

#endif