#ifndef _CARDINAL_IHD_MAIN_H_
#define _CARDINAL_IHD_MAIN_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/driver_utils.h>
#include "ihd_context.h"

#define HSW_PIPE_A_SRC 0x6001C
#define HSW_PIPE_B_SRC 0x6101C
#define HSW_PIPE_C_SRC 0x6201C

void
IHD_Init(IHD_Context *ctxt);

#endif