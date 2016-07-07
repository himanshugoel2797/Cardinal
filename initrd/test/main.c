#include <stdio.h>
#include "libs/libCardinal/include/syscall.h"

int main() {
    __asm__("hlt");
    printf("Hello World!");
    return 0;
}
