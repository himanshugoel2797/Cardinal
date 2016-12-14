#include "debug_gfx.h"
#include "boot_information/boot_information.h"
#include "bootinfo.h"
#include "font.h"
#include "common.h"
#include "utils/native.h"

void
debug_gfx_writeLine(const char *fmt, ...) {
    char str[256];
    memset(str, 0, 256);


    va_list vl;
    va_start(vl, fmt);
    vsnprintf(str, fmt, vl);
    va_end(vl);

    int i = 0;

    while(str[i])
        outb(0x3f8, str[i++]);

}
