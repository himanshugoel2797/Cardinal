#include "ihd.h"

#include <string.h>

static IHD_Context ctxt;

uint32_t
IHD_Read32(uint32_t off){
	return ctxt.iobase[off];
}

void
IHD_Write32(uint32_t off, uint32_t val) {
	ctxt.iobase[off] = val;
}

void
IHD_Init(IHD_Context *ctxt_p) {
	memcpy(&ctxt, ctxt_p, sizeof(IHD_Context));
	IHD_Write32(HSW_PIPE_A_SRC, 1920 << 16 | 1080);
}