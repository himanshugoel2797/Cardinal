/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "main.h"
#include "common.h"
#include "types.h"
#include "kmalloc.h"
#include "managers.h"
#include "memory.h"
#include "syscall.h"
#include "initrd.h"
#include "synchronization.h"
#include "timer.h"
#include "thread.h"
#include "file_server.h"
#include "boot_information/boot_information.h"

void
load_exec(UID pid, const char *exec);

void
idle_main(void);

void
kernel_main_init(void) {
    //__asm__(".cont:\n\tmov %rsp, %rax\n\tmov %rsp, %rbx\n\tint $34\n\tsub %rsp, %rax\n\tjz .cont\n\thlt");
    InitializeTimer();
    SetTimerEnableMode(ENABLE);

    kmalloc_init ();
    ProcessSys_Initialize();
    Thread_Initialize();
    KeyMan_Initialize();
    RegisterCore(NULL);

    CreateThread(ROOT_PID, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)kernel_main, NULL);

    seed(GetTimerValue());
    InterruptMan_Initialize();

    SyscallMan_Initialize();
    Syscall_Initialize();
    smp_unlock_cores();

    UID cpid = 0;
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();
    
    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main, NULL);
    StartProcess(cpid);

    //load_exec(cpid, "userboot.bin");
    SetupPreemption();

    while(1)
        ;
}

void
load_exec(UID pid, const char *exec) {
    void *exec_loc = NULL;
    uint64_t exec_size = 0;

    Initrd_GetFile(exec, &exec_loc, &exec_size);

    uint64_t orig_exec_size = exec_size;
    exec_size += EXEC_ENTRY_POINT % PAGE_SIZE;

    if(exec_size % PAGE_SIZE)
        exec_size += (PAGE_SIZE - exec_size % PAGE_SIZE);

    //Map the executable into the process
    ProcessInformation *pinfo = NULL;
    if(GetProcessReference(pid, &pinfo) != ProcessErrors_None)
        return;

    for(uint32_t i = 0; i < exec_size / PAGE_SIZE; i++) {

        uint64_t p_addr = AllocatePhysicalPage();
        uint64_t v_addr = (EXEC_ENTRY_POINT - EXEC_ENTRY_POINT % PAGE_SIZE) + i * PAGE_SIZE;

        MapPage(pinfo->PageTable,
                p_addr,
                v_addr,
                PAGE_SIZE,
                CachingModeWriteBack,
                MemoryAllocationType_Application,
                MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_User | MemoryAllocationFlags_Exec | MemoryAllocationFlags_Present);

    }


    uint8_t* write_target = (uint8_t*)SetupTemporaryWriteMap(pinfo->PageTable,
                            EXEC_ENTRY_POINT - (EXEC_ENTRY_POINT % PAGE_SIZE),
                            exec_size);

    memcpy(write_target + (EXEC_ENTRY_POINT % PAGE_SIZE), exec_loc, orig_exec_size);

    UninstallTemporaryWriteMap((uint64_t)write_target, exec_size);

    CreateThread(pid, ThreadPermissionLevel_User, (ThreadEntryPoint)EXEC_ENTRY_POINT, NULL);
    StartProcess(pid);
    return;
}

static volatile _Atomic int smp_core_count = 0;

#include "debug_gfx.h"

void
kernel_main(void) {
    while(1)
        WakeReadyThreads();
}

void
idle_main2(void) {
    while(1);
}

void
idle_main(void) {

    UID cpid = 0;
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main2, NULL);
    StartProcess(cpid);


    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main2, NULL);
    StartProcess(cpid);

    while(1);
}

void
smp_core_main(int (*getCoreData)(void)) {

    Syscall_Initialize();

    RegisterCore(getCoreData);
    smp_core_count++;
        UID cpid = 0;
        if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
            HaltProcessor();

        CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main, NULL);
        StartProcess(cpid);

        if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
            HaltProcessor();

        CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main, NULL);
        StartProcess(cpid);

    SetupPreemption();
    while(1);
}
