#include "initrd.h"
#include "program.h"
#include <cardinal/process.h>


int
LoadProgram(char *name) {
    void *elf_loc = NULL;
    size_t elf_sz = 0;
    if(GetFile(name, &elf_loc, &elf_sz)) {

        UID pid = 0;
        R0_CreateProcess(GetCurrentProcessUID(), 0, &pid);

        const char *argv[] = {name};
        return LoadAndStartApplication(pid, elf_loc, elf_sz, argv, 1);
    }
    return -1;
}

__attribute__((section(".entry_point")))
int _start() {

    ImportInitrd();

    int err = LoadProgram("elf_server.elf");
    if(err != 0)
        __asm__("hlt");

    err = LoadProgram("sys_init.elf");
    if(err != 0)
        __asm__("hlt");

    //Now exit the process, allowing the init process to take control of the system
    while(1)
        R0_ExitProcess(0);
}