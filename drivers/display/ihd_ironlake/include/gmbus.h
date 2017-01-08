#ifndef _IHD_GMBUS_H_
#define _IHD_GMBUS_H_

#include <cardinal/cardinal_types.h>

#include "ihd_regs.h"

int
GMBUS_I2C_Read(GMBUS_DEVICE device, int offset, int len, uint8_t *buf);

#endif