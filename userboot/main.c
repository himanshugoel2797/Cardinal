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

    //Bring up memory management services
    int err = LoadProgram("mem_srv.elf");
    if(err != 0)
        __asm__("hlt");

    //Bring up the service/namespace directory
    err = LoadProgram("namespace_dir.elf");
    if(err != 0)
        __asm__("hlt");

    //Bring up the process server
    err = LoadProgram("proc_srv.elf");
    if(err != 0)
        __asm__("hlt");

    //Bring up the elf server
    err = LoadProgram("elf_srv.elf");
    if(err != 0)
        __asm__("hlt");

    //Begin system initialization
    err = LoadProgram("sys_init.elf");
    if(err != 0)
        __asm__("hlt");

    //Now exit the process, allowing the init process to take control of the system
    R0_ExitProcess(0);
    while(1);
}