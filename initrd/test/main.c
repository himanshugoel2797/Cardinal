#include "libs/libCardinal/include/syscall.h"

int main() {

	uint64_t a = Syscall1(Syscall_IOPL, 3);

    while(a) __asm__("cli\n\thlt");

    return 0;
}
