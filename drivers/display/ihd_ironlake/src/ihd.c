#include "ihd.h"

#include <string.h>

static IHD_Context ctxt;

void
IHD_Init(IHD_Context *ctxt_p) {
	memcpy(&ctxt, ctxt_p, sizeof(IHD_Context));

	
}