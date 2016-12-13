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

    memcpy(vAddr, result, (buf_len > res_len)?res_len : buf_len);

    Display_Update(disp_fd, write_key, buf_len);

    Display_UnlockDisplay(disp_fd);

}