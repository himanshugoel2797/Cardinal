#include <cardinal/proc/server.h>
#include "ps2.h"

int main() {

	//Request IO privileges
	SetProperty(CardinalProperty_IOPL, 0, 3);

	//Initialize the PS/2 devices and load their drivers if available
	PS2_Initialize();

	//Surrender IOPL
	SetProperty(CardinalProperty_IOPL, 0, 0);

	while(1);
}