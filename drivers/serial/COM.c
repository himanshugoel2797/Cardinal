#include "COM.h"

#include <stdarg.h>
#include "utils/native.h"
#include "utils/common.h"
#include "processors.h"

#define PORT 0x03F8
bool com_redirect = FALSE;

void COM_Initialize()
{
#if COM_ENABLED == 1
    outb(PORT + 1, 0x00); // Disable all interrupts
    outb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00); //                  (hi byte)
    outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
#endif
}

uint8_t COM_ready()
{
#if COM_ENABLED == 1
    return inb(PORT + 5) & 0x20;
#else
    return 1;
#endif

}
int ln = 0;
void COM_WriteStr(const char *fmt, ...)
{
    char str[1024];
    int index = 0;
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(str, fmt, vl);
    va_end(vl);

    if(com_redirect)
        {
            Terminal_Write(str, strlen(str));
        }
#if COM_ENABLED == 1
    while(str[index])
        {
            while(COM_ready() == 0) ;
            outb(PORT, str[index++]);
        }
#endif
}
