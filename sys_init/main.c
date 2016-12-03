#include "initrd.h"

int _start() {

	__asm__("hlt");
    //First map in the initrd
    ImportInitrd();

    //Use it along with the loaded elf loader to load the initial system processes

    //Wait until exit requested on power off
    while(1);
}