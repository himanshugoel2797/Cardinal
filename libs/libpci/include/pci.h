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