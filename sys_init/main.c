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
    uint64_t read_key = 0, write_key = 0;
    IO_AllocateBuffer(&buf_len,
                      &vAddr,
                      &read_key,
                      &write_key);

    uint8_t *display_addr = (uint8_t*)vAddr;
    uint8_t *src_img = (uint8_t*)result;

    for(int y = 0; y < img_h; y++) {
        for(int x = 0; x < img_w; x++) {

            display_addr[y * disp_info.pitch + (disp_info.bpp/8 * x)] = src_img[y * img_p + (4 * x)];
            display_addr[y * disp_info.pitch + (disp_info.bpp/8 * x) + 1] = src_img[y * img_p + (4 * x) + 1];
            display_addr[y * disp_info.pitch + (disp_info.bpp/8 * x) + 2] = src_img[y * img_p + (4 * x) + 2];
            display_addr[y * disp_info.pitch + (disp_info.bpp/8 * x) + 3] = src_img[y * img_p + (4 * x) + 3];

        }
    }

    Display_Update(disp_fd, write_key, buf_len);

    Display_UnlockDisplay(disp_fd);

    //Wait until exit requested on power off
    while(1);
}