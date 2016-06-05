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