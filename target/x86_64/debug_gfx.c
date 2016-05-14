#include "debug_gfx.h"
#include "boot_information/boot_information.h"
#include "bootinfo.h"
#include "font.h"
#include "common.h"

static int xPos = 0, yPos = 0;

void
debug_gfx_writeLine(const char *fmt, ...) {
    char str[256];
    memset(str, 0, 256);


    va_list vl;
    va_start(vl, fmt);
    vsnprintf(str, fmt, vl);
    va_end(vl);

    CardinalBootInfo *b = GetBootInfo();

    int yOff = yPos, xOff = xPos;
    int pitch = b->framebuffer_pitch;

    uint32_t *backBuffer = (uint32_t*)b->framebuffer_addr;

    for (int i = 0; str[i] != 0; i++)
        for (int b = 0; b < 8; b++) {
            for (int a = xOff; a < xOff + 13; a++) {
                backBuffer[(yOff + (8 - b) + (a * pitch))] =
                    (((letters[str[i] - 32][13 - (a - xOff)] >> b) & 1)) *
                    (0xDDDDDDDD);
            }
            yOff += 8;
        }

    xPos = xOff;
    yPos = yOff;
}
