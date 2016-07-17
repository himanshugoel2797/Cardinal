#include "main.h"
#include "types.h"
#include "kmalloc.h"
#include "managers.h"
#include "memory.h"
#include "program.h"
#include "syscall.h"
#include "initrd.h"
#include "synchronization.h"
#include "timer.h"

Spinlock smp_lock;

void
sleep_kernel(void) {
    __asm__("cli\n\thlt" :: "a"(0xDEADB33F));
}

void
hlt_kernel(void) {
    while(1);
}

void
hlt2_kernel(void) {
    while(1);
}
void
kernel_main_init(void) {
    //__asm__(".cont:\n\tmov %rsp, %rax\n\tmov %rsp, %rbx\n\tint $34\n\tsub %rsp, %rax\n\tjz .cont\n\thlt");
    InitializeTimer();
    SetTimerValue(0);
    SetTimerEnableMode(ENABLE);

    kmalloc_init ();
    ProcessSys_Initialize();
    Thread_Initialize();
    RegisterCore(0, NULL);
    CreateThread(0, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)kernel_main, NULL);
    CoreUpdate();  //BSP is core 0
}

void
load_elf(void) {
    __asm__("cli\n\thlt");
    void *elf_loc = NULL;
    uint64_t elf_size = 0;

    Initrd_GetFile("test.elf", &elf_loc, &elf_size);
    const char *argv[] = {"test.elf"};

    LoadAndStartApplication(elf_loc, elf_size, argv, 1, NULL);
    while(1);
}

volatile int coreCount = 0;

void
kernel_main(void) {

    //Initialize process manager, setup timers, get threading up and running
    // Enumerate and initialize drivers
    // Load UI elf from disk
    // Switch to usermode
    // Execute UI

    coreCount++;
    SyscallMan_Initialize();
    Syscall_Initialize();

    SetupSecurityMonitor();

    DeviceManager_Initialize();
    smp_lock = CreateSpinlock();
    smp_unlock_cores();

    while(coreCount != GetCoreCount());
    setup_preemption();
    target_device_setup();

    ProcessInformation p_info;
    GetProcessInformation(0, &p_info);
    ProcessInformation *elf_proc;
    ForkProcess(&p_info, &elf_proc);
    if(!CreateThread(elf_proc->ID, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)load_elf, NULL))__asm__("cli\n\thlt");
    //CreateThread(0, hlt2_kernel);

    FreeThread(GetCurrentThreadUID());
    while(1);
}


void
smp_main(void) {
    LockSpinlock(smp_lock);
    coreCount++;
    UnlockSpinlock(smp_lock);
    setup_preemption();
    FreeThread(GetCurrentThreadUID());
    while(1);
}

void
smp_core_main(int coreID,
              int (*getCoreData)(void)) {
    getCoreData = NULL;
    Syscall_Initialize();
    RegisterCore(coreID, getCoreData);
    CreateThread(0, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)smp_main, NULL);
    CoreUpdate();
    while(1);
    //Start the local timer and set it to call the thread switch handler
}