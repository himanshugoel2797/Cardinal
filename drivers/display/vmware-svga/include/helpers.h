/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_VMWARE_SVGA_HELPERS_H_
#define _CARDINAL_VMWARE_SVGA_HELPERS_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/driver_utils.h>

#include "svga.h"
#include "svga_reg.h"

static __inline uint32_t
SVGA_Read32(SVGA_Context *ctxt, uint32_t off) {
    outw(ctxt->iobase + SVGA_INDEX_PORT, off);
    return inw(ctxt->iobase + SVGA_VALUE_PORT);
}

static __inline void
SVGA_Write32(SVGA_Context *ctxt, uint32_t off, uint32_t val) {
    outw(ctxt->iobase + SVGA_INDEX_PORT, off);
    outw(ctxt->iobase + SVGA_VALUE_PORT, val);
}

#endif