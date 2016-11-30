#include <cardinal/process.h>

__attribute__((section(".entry_point")))
int _start() {

    //Register to the program loader target

    //Poll for a load request

    /*	void *elf_loc = NULL; size_t elf_sz = 0;
    	if(GetFile("namespace_man.elf", &elf_loc, &elf_sz)) {

    		UID pid = 0;
    		R0_CreateProcess(GetCurrentProcessUID(), 0, &pid);

    		const char *argv[] = {"namespace_man.elf"};
        	LoadAndStartApplication(pid, elf_loc, elf_sz, argv, 1);
    	}*/

    while(1);
}