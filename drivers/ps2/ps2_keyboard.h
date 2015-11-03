#ifndef _PS2_KEYBOARD_H_
#define _PS2_KEYBOARD_H_

#include "types.h"


uint8_t PS2Keyboard_Initialize();
void PS2Keyboard_SetLEDStatus(uint8_t led, uint8_t status);

#endif /* end of include guard: _PS2_KEYBOARD_H_ */
