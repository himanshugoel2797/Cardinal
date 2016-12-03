#include "initrd.h"
#include "program.h"
#include <string.h>
#include <cardinal/process.h>
#include <procserver/server.h>

int
LoadProgram(char *name) {
    void *elf_loc = NULL;
    size_t elf_sz = 0;
    if(GetFile(name, &elf_loc, &elf_sz)) {

        UID pid = 0;

        char *argv[] = {name};
        static int call_cnt = 0;
        if(call_cnt++){
            return RequestCreateProcess(elf_loc, elf_sz, argv, 1, &pid);
            //R0NotifyProcessExistence(pid, name, strlen(name));
        }

        R0_CreateProcess(GetCurrentProcessUID(), 0, &pid);
        return LoadAndStartApplication(pid, elf_loc, elf_sz, (const char**)argv, 1);
    }
    return -1;
}

__attribute__((section(".entry_point")))
int _start() {

    ImportInitrd();

    //Bring up the process server
    int err = LoadProgram("proc_server.elf");
    if(err != 0)
        __asm__("hlt");

    //Bring up memory management services
    err = LoadProgram("mem_server.elf");
    if(err != 0)
        __asm__("hlt");

    //Bring up the service/namespace directory
    err = LoadProgram("namespace_dir.elf");
    if(err != 0)
        __asm__("hlt");

    //Begin system initialization
    err = LoadProgram("framebuffer.elf");
    if(err != 0)
        __asm__("hlt");

    //Now exit the process, allowing the init process to take control of the system
    R0_ExitProcess(0);
    while(1);
}