/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _IHDA_H_
#define _IHDA_H_

#define IHDA_VMIN_REG 0x02
#define IHDA_VMAJ_REG 0x03
#define IHDA_OUTPAY_REG 0x04

#define IHDA_GCTL_REG 0x08
#define IHDA_STATESTS_REG 0x0e

#define IHDA_CORB_LO_REG 0x40
#define IHDA_CORB_HI_REG 0x44
#define IHDA_CORB_WRITE_REG 0x48
#define IHDA_CORB_READ_REG 0x4a
#define IHDA_CORB_CTRL_REG 0x4c
#define IHDA_CORB_STS_REG 0x4d
#define IHDA_CORB_SZ_REG 0x4e

#define IHDA_RIRB_LO_REG 0x50
#define IHDA_RIRB_HI_REG 0x54
#define IHDA_RIRB_WRITE_REG 0x58
#define IHDA_RIRB_INTCNT_REG 0x5a
#define IHDA_RIRB_CTRL_REG 0x5c
#define IHDA_RIRB_STS_REG 0x5d
#define IHDA_RIRB_SZ_REG 0x5e

#define IHDA_DMA_LO_REG 0x70
#define IHDA_DMA_HI_REG 0x74

#define IHDA_SDnCTRL_REG (n) (0x80 + n * 0x20)
#define IHDA_SDnSTS_REG (n) (0x83 + n * 0x20)

#define IHDA_8B_SUP_BIT (1 << 4)
#define IHDA_64B_SUP_BIT (1 << 5)
#define IHDA_1024B_SUP_BIT (1 << 6)

#define IHDA_8B_VAL 0x00
#define IHDA_64B_VAL 0x01
#define IHDA_1024B_VAL 0x02

#define IHDA_CORB_RUN (1 << 1)
#define IHDA_RIRB_RUN (1 << 1)

#define IHDA_GCTL_RESET 1

#define IHDA_MINOR_VER 0x00
#define IHDA_MAJOR_VER 0x01
#define IHDA_OUTPAY_RESETVAL 0x3C

#define IHDA_VERB(codec, node, payload) ((codec << 28) | (node << 20) | (payload))
#define IHDA_CORB_VERB(codec, node, cmd, data) IHDA_VERB(codec, node, ((cmd << 8) | (data)))

typedef enum {
    CMD_GetParameter = 0xf00,
    CMD_GetSelectedInput = 0xf01,
    CMD_SetSelectedInput = 0x701,
} IHDA_CORB_CMDs;

#endif