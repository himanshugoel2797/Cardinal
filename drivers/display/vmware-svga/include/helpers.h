#ifndef _CARDINAL_VMWARE_SVGA_HELPERS_H_
#define _CARDINAL_VMWARE_SVGA_HELPERS_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/driver_utils.h>

#include "svga.h"
#include "svga_reg.h"

static __inline uint32_t
SVGA_Read32(SVGA_Context *ctxt, uint32_t off){
	outw(ctxt->iobase + SVGA_INDEX_PORT, off);
	return inw(ctxt->iobase + SVGA_VALUE_PORT);
}

static __inline void
SVGA_Write32(SVGA_Context *ctxt, uint32_t off, uint32_t val){
	outw(ctxt->iobase + SVGA_INDEX_PORT, off);
	outw(ctxt->iobase + SVGA_VALUE_PORT, val);
}

#endif