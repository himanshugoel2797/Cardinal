#ifndef _CARDINAL_VMWARE_SVGA_SVGA_H_
#define _CARDINAL_VMWARE_SVGA_SVGA_H_

#include <cardinal/cardinal_types.h>
#include <pci/pci.h>

typedef struct {
	PCI_Device device;

	uint64_t iobase;
	uint64_t fbmem;
	uint32_t *fifomem;

	uint64_t fifosz;
	uint64_t fbsz;
	uint64_t vramsz;

	uint32_t devVer;
	uint32_t caps;

	uint32_t width;
	uint32_t height;
	uint32_t bpp;
	uint32_t pitch;
} SVGA_Context;

void
SVGA_Init(SVGA_Context *ctxt);

void
SVGA_Enable(SVGA_Context *ctxt);

void
SVGA_SetMode(SVGA_Context *ctxt,
			 uint32_t width,
			 uint32_t height,
			 uint32_t bpp);

#endif