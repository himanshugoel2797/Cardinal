#include "initrd.h"
#include <libio/fileserver.h>

int main() {
    //First map in the initrd
    ImportInitrd();

    //Use it along with the loaded elf loader to load the initial system processes
    uint8_t key[KEY_BYTES];
    uint64_t fd = 0;

    UID pid = 0;
    uint64_t error = 0;

    while(1){
    	uint32_t tmp_key = 0;
    	RetrieveNamespace("disp0", &tmp_key);
    	while(!IsNamespaceRetrieved(tmp_key, &pid, &error));

    	IO_Open("fbuf", 0, 0, key, pid, fd);
	}

    //Wait until exit requested on power off
    while(1);
}