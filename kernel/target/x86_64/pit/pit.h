/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _PIT_H_
#define _PIT_H_

#include "types.h"

#define PIT_CH0_DATA 0x40
#define PIT_CH2_DATA 0x42
#define PIT_CMD      0x43

#define PIT_CH0      0x0
#define PIT_CH2      0x2

#define PIT_ACCESS_LATCH 0x0
#define PIT_ACCESS_LO_BYTE 0x1
#define PIT_ACCESS_HI_BYTE 0x2

#define PIT_MODE_INTERRUPT 0
#define PIT_MODE_ONESHOT 1
#define PIT_MODE_RATE 2
#define PIT_MODE_SQUARE_WAVE 3
#define PIT_MODE_SOFT_STROBE 4
#define PIT_MODE_HARD_STROBE 5

#define PIT_VAL_16BIT 0
#define PIT_VAL_BCD   1

#define PIT_FREQUENCY_HZ 1000
//#define PIT_FREQUENCY_HZ 50

void
PIT_Initialize(void);

//Prevent public access to this function
void
PIT_SetFrequency(uint8_t channel,
                 uint8_t access,
                 uint8_t mode,
                 uint8_t valType,
                 uint32_t frequency);

uint32_t
PIT_GetFrequency(void);

void
PIT_Sleep(uint32_t interval);

void
PIT_SetEnableMode(bool enabled);

#endif /* end of include guard: _PIT_H_ */