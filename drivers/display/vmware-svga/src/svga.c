#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include "svga.h"
#include "svga_reg.h"

#include "helpers.h"

void
SVGA_Init(SVGA_Context *ctxt) {

	ctxt->devVer = SVGA_ID_2;
	do {
    	SVGA_Write32(ctxt, SVGA_REG_ID, ctxt->devVer);
    	if (SVGA_Read32(ctxt, SVGA_REG_ID) == ctxt->devVer) {
    		break;
    	} else {
    		ctxt->devVer--;
    	}
	} while (ctxt->devVer >= SVGA_ID_0);

	ctxt->vramsz = SVGA_Read32(ctxt, SVGA_REG_VRAM_SIZE);
	ctxt->fbsz = SVGA_Read32(ctxt, SVGA_REG_FB_SIZE);
	ctxt->fifosz = SVGA_Read32(ctxt, SVGA_REG_MEM_SIZE);

	if(ctxt->devVer >= SVGA_ID_1)
		ctxt->caps = SVGA_Read32(ctxt, SVGA_REG_CAPABILITIES);

	SVGA_Enable(ctxt);
}

void
SVGA_Enable(SVGA_Context *ctxt) {

	ctxt->fifomem[SVGA_FIFO_MIN] = SVGA_FIFO_NUM_REGS * sizeof(uint32_t);
	ctxt->fifomem[SVGA_FIFO_MAX] = ctxt->fifosz;
	ctxt->fifomem[SVGA_FIFO_NEXT_CMD] = ctxt->fifomem[SVGA_FIFO_MIN];
	ctxt->fifomem[SVGA_FIFO_STOP] = ctxt->fifomem[SVGA_FIFO_MIN];

	SVGA_Write32(ctxt, SVGA_REG_ENABLE, 1);
	SVGA_Write32(ctxt, SVGA_REG_CONFIG_DONE, 1);
}

void
SVGA_SetMode(SVGA_Context *ctxt,
			 uint32_t width,
			 uint32_t height,
			 uint32_t bpp) {
	ctxt->width = width;
	ctxt->height = height;
	ctxt->bpp = bpp;


	SVGA_Write32(ctxt, SVGA_REG_WIDTH, width);
	SVGA_Write32(ctxt, SVGA_REG_HEIGHT, height);
	SVGA_Write32(ctxt, SVGA_REG_BITS_PER_PIXEL, bpp);
	SVGA_Write32(ctxt, SVGA_REG_ENABLE, 1);
	ctxt->pitch = SVGA_Read32(ctxt, SVGA_REG_BYTES_PER_LINE);
}