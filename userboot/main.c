#include "initrd.h"
#include "program.h"
#include <cardinal/process.h>


int
LoadProgram(char *name) {
	void *elf_loc = NULL; size_t elf_sz = 0;
	if(GetFile(name, &elf_loc, &elf_sz)) {

		UID pid = 0;
		R0_CreateProcess(GetCurrentProcessUID(), 0, &pid);

		const char *argv[] = {name};
    	LoadAndStartApplication(pid, elf_loc, elf_sz, argv, 1);
    	return 0;
	}
	return -1;	
}

__attribute__((section(".entry_point"))) 
int _start() {

	ImportInitrd();

	LoadProgram("elf_server.elf");

    while(1);
}