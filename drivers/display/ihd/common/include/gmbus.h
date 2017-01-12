#ifndef _IHD_GMBUS_H_
#define _IHD_GMBUS_H_

#include <cardinal/cardinal_types.h>

#include "ihd_regs.h"

#define EDID_OFFSET 0x50
#define EDID_LEN 128

int
GMBUS_I2C_Read(GMBUS_DEVICE device, int offset, int len, uint8_t *buf);

int
DP_I2C_Read(int device, int offset, int len, uint8_t *buf);

int
EDID_Read(int display);

#endif