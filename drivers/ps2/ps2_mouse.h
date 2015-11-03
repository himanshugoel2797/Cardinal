#ifndef _PS2_MOUSE_DRIVER_H_
#define _PS2_MOUSE_DRIVER_H_

#include "types.h"

uint8_t
PS2Mouse_Initialize(void);

bool
PS2Mouse_IsFiveButton(void);

bool
PS2Mouse_HasScollWheel(void);

#endif