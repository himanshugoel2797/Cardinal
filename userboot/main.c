/**
 * Copyright (c) 2017 Himanshu Goel
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "initrd.h"
#include "program.h"
#include <string.h>
#include <cardinal/ipc.h>
#include <cardinal/process.h>
#include <cardinal/proc/server.h>

int
LoadProgram(char *name, UID *p_pid) {
    void *elf_loc = NULL;
    size_t elf_sz = 0;
    if(GetFile(name, &elf_loc, &elf_sz)) {

        UID pid = 0;

        char *argv[] = {name};
        static int call_cnt = 0;
        if(call_cnt++) {
            return RequestCreateProcess(elf_loc, elf_sz, argv, 1, 0, &pid);
            //R0NotifyProcessExistence(pid, name, strlen(name));
        }

        R0_CreateProcess(GetCurrentProcessUID(), 0, &pid);

        *p_pid = pid;

        return LoadAndStartApplication(pid, elf_loc, elf_sz, name, 1, strlen(name) + 1);
    }
    return -1;
}

__attribute__((section(".entry_point")))
int _start() {
    while(1)
        ;
    if(ImportInitrd() != 0)
        __asm__("hlt");


    UID pid = 0;
    CREATE_NEW_MESSAGE_PTR(msg);

    //Bring up the process server
    int err = LoadProgram("proc_server.elf", &pid);
    if(err != 0)
        __asm__("hlt");
    while(!GetIPCMessage(msg));

    //Bring up memory management services
    err = LoadProgram("mem_server.elf", &pid);
    if(err != 0)
        __asm__("hlt");
    while(!GetIPCMessage(msg));

    //Bring up the service/namespace directory
    err = LoadProgram("namespace_srv.elf", &pid);
    if(err != 0)
        __asm__("hlt");
    while(!GetIPCMessage(msg));

    err = LoadProgram("sibyl.elf", &pid);
    if(err != 0)
        __asm__("hlt");

    err = LoadProgram("initrd_srv.elf", &pid);
    if(err != 0)
        __asm__("hlt");
    while(!GetIPCMessage(msg));

    err = LoadProgram("pci_loader.elf", &pid);
    if(err != 0)
        __asm__("hlt");
    while(!GetIPCMessage(msg));

    err = LoadProgram("framebuffer.elf", &pid);
    if(err != 0)
        __asm__("hlt");
    while(!GetIPCMessage(msg));

    //Begin system initialization
    err = LoadProgram("sys_init.elf", &pid);
    if(err != 0)
        __asm__("hlt");

    R0_ExitProcess(0);
    while(1);

    //Now exit the process, allowing the init process to take control of the system
}