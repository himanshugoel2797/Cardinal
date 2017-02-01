/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/io/server.h>
#include <cardinal/namespace/server.h>
#include <string.h>
#include <stdio.h>

#include <display/display.h>

#include "initrd.h"
#include "png.h"

int main() {
    //First map in the initrd
    ImportInitrd();

    Display_Initialize();

    int disp_cnt = Display_GetDisplayCount();

    DisplayInfo disp_info;
    Display_GetInfo(0, &disp_info);
    uint64_t disp_fd = Display_TryLockDisplay(0);

    void *file_loc = NULL;
    size_t file_size = 0;
    GetFile("wallpaper.png", &file_loc, &file_size);

    int img_w = 0;
    int img_h = 0;
    int img_p = 0;
    int res_len = 0;

    void *result = DecodePNGtoRGBA(file_loc, file_size, &img_w, &img_h, &img_p, &res_len);

    uint64_t buf_len = disp_info.pitch * disp_info.height;
    uint64_t vAddr = 0;
    Key_t read_key, write_key;
    memset(&read_key, 0, sizeof(Key_t));
    memset(&write_key, 0, sizeof(Key_t));

    IO_AllocateBuffer(&buf_len,
                      &vAddr,
                      &read_key,
                      &write_key);

    uint8_t *display_addr = (uint8_t*)vAddr;
    uint8_t *src_img = (uint8_t*)result;

    for(int y = 0; y < disp_info.height; y++) {
        for(int x = 0; x < disp_info.width; x++) {
            display_addr[y * disp_info.pitch + (4 * x)] = src_img[y * img_p + (4 * x)];
            display_addr[y * disp_info.pitch + (4 * x) + 1] = src_img[y * img_p + (4 * x) + 1];
            display_addr[y * disp_info.pitch + (4 * x) + 2] = src_img[y * img_p + (4 * x) + 2];
            display_addr[y * disp_info.pitch + (4 * x) + 3] = src_img[y * img_p + (4 * x) + 3];
        }
    }

    Display_Update(disp_fd, write_key, buf_len);

    Display_UnlockDisplay(disp_fd);

    //Wait until exit requested on power off
    while(1);
}