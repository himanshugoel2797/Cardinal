/**
 * Copyright (c) 2017 Himanshu Goel
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
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

static volatile _Atomic int smp_core_count = 0;

void
load_exec(UID pid, const char *exec);

void
idle_main(void);

void
kernel_main_init(void) {
    //__asm__(".cont:\n\tmov %rsp, %rax\n\tmov %rsp, %rbx\n\tint $34\n\tsub %rsp, %rax\n\tjz .cont\n\thlt");
    InitializeTimer();
    SetTimerEnableMode(ENABLE);
    PrintDebugMessage("Timer initialized.\r\n");

    kmalloc_init ();
    PrintDebugMessage("kmalloc initialized.\r\n");

    Thread_Initialize();
    PrintDebugMessage("Threading initialized.\r\n");

    KeyMan_Initialize();
    PrintDebugMessage("Key management initialized.\r\n");

    RegisterCore(NULL);

    UID tid = CreateThread(ROOT_PID, FALSE, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)kernel_main, NULL);
    SetThreadState(tid, ThreadState_Initialize);

    seed(GetTimerValue());
    InterruptMan_Initialize();

    SyscallMan_Initialize();
    Syscall_Initialize();

    smp_core_count = 1;
    smp_unlock_cores();

    while(smp_core_count != GetCoreCount())
        ;
    
    //tid = CreateThread(ROOT_PID, FALSE, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main, NULL);
    //SetThreadState(tid, ThreadState_Initialize);


    //load_exec(ROOT_PID, "userboot.bin");
    SetupPreemption();

    while(1)
        ;
}

void
load_exec(UID pid, const char *exec) {
    
    UID tid = CreateThread(pid, TRUE, ThreadPermissionLevel_User, (ThreadEntryPoint)EXEC_ENTRY_POINT, NULL);
    
    void *exec_loc = NULL;
    uint64_t exec_size = 0;

    Initrd_GetFile(exec, &exec_loc, &exec_size);

    uint64_t orig_exec_size = exec_size;
    exec_size += EXEC_ENTRY_POINT % PAGE_SIZE;

    if(exec_size % PAGE_SIZE)
        exec_size += (PAGE_SIZE - exec_size % PAGE_SIZE);

    //Map the executable into the process
    ProcessInfo *pinfo = NULL;
    if(GetProcessReference(tid, &pinfo) != ThreadError_None)
        return;

    if(LockSpinlock(pinfo->lock) == NULL){
        ReturnProcessReference(tid);
        return;
    }

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

    UnlockSpinlock(pinfo->lock);
    ReturnProcessReference(pid);

    SetThreadState(tid, ThreadState_Initialize);
    return;
}

void
kernel_main(void) {
    while(1)
        PrintDebugMessage("KERNEL_MAIN\r\n");
        ;   
        //WakeReadyThreads();
}

void
idle_main2(void) {
    while(1);
}

void
idle_main(void) {

    /*UID cpid = 0;
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main2, NULL);
    StartProcess(cpid);


    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main2, NULL);
    StartProcess(cpid);
    */
    while(1)
        ;   //PrintDebugMessage("IDLE_MAIN\r\n");;
}

void
smp_core_main(int (*getCoreData)(void)) {

    Syscall_Initialize();

    RegisterCore(getCoreData);
    smp_core_count++;
    /*UID cpid = 0;
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main, NULL);
    StartProcess(cpid);
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main, NULL);
    StartProcess(cpid);

    */
    SetupPreemption();
    while(1);
}
