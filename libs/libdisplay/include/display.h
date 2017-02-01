/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LIB_DISPLAY_H_
#define _LIB_DISPLAY_H_

#include <cardinal/cardinal_types.h>

#define DISPLAY_INFO_STRING "Device Name: %s\nWidth: %d\nHeight: %d\nPitch: %d\nBit Depth: %d\nR Offset: %d R Length: %d\nG Offset: %d G Length %d\nB Offset: %d B Length: %d\nA Offset: %d A Length: %d\n"

typedef struct {
    int width;
    int height;
    int pitch;
    int bpp;

    int R_offset;
    int R_length;

    int G_offset;
    int G_length;

    int B_offset;
    int B_length;

    int A_offset;
    int A_length;
} DisplayInfo;

int
Display_Initialize(void);

int
Display_GetDisplayCount(void);

int
Display_GetInfo(int display_index,
                DisplayInfo *disp_info);

int
Display_GetDisplayImage(int display_index,
                        Key_t key,
                        uint64_t len);

uint64_t
Display_TryLockDisplay(int display_index);

int
Display_UnlockDisplay(uint64_t display_fd);

void
Display_Update(uint64_t display_fd,
               Key_t buffer_key,
               uint64_t buffer_len);

#endif