/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/cardinal_types.h>
#include <stdlib.h>
#include "png.h"

static uint32_t file_pos = 0;
static uint32_t file_len = 0;

static void
read(char *src, char *dst, int len) {

    for(int i = 0; i < len; i++)
        dst[i] = src[file_pos + i];

    file_pos += len;
}

void
ReadDataFromInputStream(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if(io_ptr == NULL)
        return;   // add custom error handling here

    read((char*)io_ptr, (char*)outBytes, byteCountToRead);
}

void*
DecodePNGtoRGBA(void *src, int len, int *img_w, int *img_h, int *img_p, int *res_len) {

    file_pos = 0;
    file_len = len;

    char sig[8];
    read((char*)src, sig, 8);

    if(!png_check_sig(sig, 8))
        return NULL;

    // get PNG file info struct (memory is allocated by libpng)
    png_structp png_ptr = NULL;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(png_ptr == NULL)
        return NULL;

    // get PNG image data info struct (memory is allocated by libpng)
    png_infop info_ptr = NULL;
    info_ptr = png_create_info_struct(png_ptr);

    if(info_ptr == NULL) {
        // libpng must free file info struct memory before we bail
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    png_set_read_fn(png_ptr, src, ReadDataFromInputStream);

    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int bitDepth = 0;
    int colorType = -1;
    png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr,
                                      &width,
                                      &height,
                                      &bitDepth,
                                      &colorType,
                                      NULL, NULL, NULL);

    if(retval != 1)
        return NULL;	// add error handling and cleanup

    png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);

    char* rowData = malloc(bytesPerRow);
    char* data = malloc(4 * width * height);

    *img_w = width;
    *img_h = height;
    *img_p = 4 * width;
    *res_len = 4 * width * height;


    // read single row at a time
    for(uint32_t rowIdx = 0; rowIdx < height; ++rowIdx) {
        png_read_row(png_ptr, (png_bytep)rowData, NULL);

        uint32_t byteIndex = 0;
        for(uint32_t colIdx = 0; colIdx < width; ++colIdx) {
            uint32_t red   = rowData[byteIndex++];
            uint32_t green = rowData[byteIndex++];
            uint32_t blue  = rowData[byteIndex++];

            uint32_t alpha = 255;
            if(bytesPerRow / width != 3)
                alpha = rowData[byteIndex++];

            data[rowIdx * *img_p + (4 * colIdx)] = blue;
            data[rowIdx * *img_p + (4 * colIdx) + 1] = green;
            data[rowIdx * *img_p + (4 * colIdx) + 2] = red;
            data[rowIdx * *img_p + (4 * colIdx) + 3] = alpha;
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return data;
}