#include "main.h"
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
kernel_main_init(void) {
    //__asm__(".cont:\n\tmov %rsp, %rax\n\tmov %rsp, %rbx\n\tint $34\n\tsub %rsp, %rax\n\tjz .cont\n\thlt");
    InitializeTimer();
    SetTimerEnableMode(ENABLE);

    kmalloc_init ();
    ProcessSys_Initialize();
    Thread_Initialize();
    RegisterCore(0, NULL);
    CreateThread(ROOT_PID, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)kernel_main, NULL);

    CoreUpdate();  //BSP is core 0
}

void
load_exec(UID pid, const char *exec) {
    void *exec_loc = NULL;
    uint64_t exec_size = 0;

    Initrd_GetFile(exec, &exec_loc, &exec_size);

    //Map the executable into the process

    CreateThread(pid, ThreadPermissionLevel_User, (ThreadEntryPoint)EXEC_ENTRY_POINT, NULL);
    StartProcess(pid);
    while(1);
}

void
kernel_main(void) {

    //Initialize process manager, setup timers, get threading up and running
    // Enumerate and initialize drivers
    // Load UI elf from disk
    // Switch to usermode
    // Execute UI

    SyscallMan_Initialize();
    Syscall_Initialize();
    DeviceManager_Initialize();
    smp_unlock_cores();
    SetupPreemption();
    target_device_setup();

    UID cpid = 0;
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    load_exec(cpid, "userboot.bin");

    FreeThread(GetCurrentThreadUID());
    while(1);
}

void
smp_core_main(int coreID,
              int (*getCoreData)(void)) {
    getCoreData = NULL;
    coreID = 0;

    //Expose additional cores as a service
    Syscall_Initialize();
    __asm__ volatile("sti");
    while(1);
    //Start the local timer and set it to call the thread switch handler
}
