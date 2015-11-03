#ifndef _PS2_CTRL_H_
#define _PS2_CTRL_H_

#include "types.h"

#include "ps2_keyboard.h"
#include "ps2_mouse.h"

uint8_t PS2_Initialize();
uint8_t PS2_ReadStatus();
uint8_t PS2_ReadConfig();
void PS2_WriteConfig(uint8_t cfg);

#endif /* end of include guard: _PS2_CTRL_H_ */
