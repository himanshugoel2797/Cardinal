#include "initrd.h"

int _start() {

    //First map in the initrd
    ImportInitrd();

    //Use it along with the linked in elf loader to load the initial system processes

    //Wait until exit requested on power off
    while(1);
}