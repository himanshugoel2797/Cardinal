/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "ihd.h"
#include "gmbus.h"

#include <string.h>

static IHD_Context ctxt;

uint32_t
IHD_Read32(uint32_t off) {
    return ctxt.iobase[off / sizeof(uint32_t)];
}

void
IHD_Write32(uint32_t off, uint32_t val) {
    ctxt.iobase[off / sizeof(uint32_t)] = val;
}

void
IHD_Init(IHD_Context *ctxt_p) {
    memcpy(&ctxt, ctxt_p, sizeof(IHD_Context));

    uint8_t edid[128];
    GMBUS_I2C_Read(GMBUS_DEVICE_LVDS, 0x50, 128, edid);

    //Backlight ctrl
    uint32_t val = IHD_Read32(0x48254);
    val &= 0xFFFF0000;
    IHD_Write32(0x48254, val | 0x10);
}