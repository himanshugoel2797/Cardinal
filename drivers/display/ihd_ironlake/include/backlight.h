#ifndef _IHD_BACKLIGHT_H_
#define _IHD_BACKLIGHT_H_

#include <cardinal/cardinal_types.h>
#include <stdbool.h>

void
Backlight_SetPWMActiveState(int index, bool active);

int
Backlight_GetMaxBacklightBrightness(int index);

void
Backlight_SetBacklightBrightness(int index, int val);

void
Backlight_SetActiveState(int index, bool active);

#endif