/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

        return LoadAndStartApplication(pid, elf_loc, elf_sz, (const char**)argv, 1);
    }
    return -1;
}

__attribute__((section(".entry_point")))
int _start() {
    while(1);
    ImportInitrd();

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

    err = LoadProgram("framebuffer.elf", &pid);
    if(err != 0)
        __asm__("hlt");

    err = LoadProgram("pci_loader.elf", &pid);
    if(err != 0)
        __asm__("hlt");

    //Begin system initialization
    err = LoadProgram("sys_init.elf", &pid);
    if(err != 0)
        __asm__("hlt");

    R0_ExitProcess(0);
    while(1);

    //Now exit the process, allowing the init process to take control of the system
}