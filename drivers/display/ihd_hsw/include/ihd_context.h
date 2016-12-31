#ifndef _CARDINAL_IHD_CONTEXT_H_
#define _CARDINAL_IHD_CONTEXT_H_

#include <cardinal/cardinal_types.h>

#include <pci/pci.h>

typedef struct {
	PCI_Device device;
	uint32_t *iobase;
} IHD_Context;

#endif