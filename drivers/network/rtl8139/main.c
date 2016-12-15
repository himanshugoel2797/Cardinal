#include <cardinal/driver_utils.h>

int main(int argc, char *argv[]) {

	R01_GetIOPrivileges();	

	int i = 0;
	while(argv[0][i])
		outb(0x3f8, argv[0][i++]);

	__asm__("hlt" :: "a"(argc));
}