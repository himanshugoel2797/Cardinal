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
#include "thread.h"
#include "file_server.h"
#include "boot_information/boot_information.h"

void
kernel_main_init(void) {
    //__asm__(".cont:\n\tmov %rsp, %rax\n\tmov %rsp, %rbx\n\tint $34\n\tsub %rsp, %rax\n\tjz .cont\n\thlt");
    InitializeTimer();
    SetTimerEnableMode(ENABLE);

    {
        CardinalBootInfo *info = GetBootInfo();
        for(uint32_t y = 0; y < info->framebuffer_height * info->framebuffer_pitch; y+=4) {
            *(uint32_t*)(info->framebuffer_addr + y) = -1;  //ARGB
        }
    }
    kmalloc_init ();
    ProcessSys_Initialize();
    Thread_Initialize();
    RegisterCore(0, NULL);
    CreateThread(ROOT_PID, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)kernel_main, NULL);

    {
        CardinalBootInfo *info = GetBootInfo();
        for(uint32_t y = 0; y < info->framebuffer_height * info->framebuffer_pitch; y+=4) {
            *(uint32_t*)(info->framebuffer_addr + y) = (uint32_t)(-1) << 8;  //ARGB
        }
    }

    CoreUpdate();  //BSP is core 0
}

void
load_elf(const char *exec) {
    void *elf_loc = NULL;
    uint64_t elf_size = 0;

    Initrd_GetFile(exec, &elf_loc, &elf_size);
    const char *argv[] = {exec};



    LoadAndStartApplication(elf_loc, elf_size, argv, 1, NULL);
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

    {
        CardinalBootInfo *info = GetBootInfo();
        for(uint32_t y = 0; y < info->framebuffer_height * info->framebuffer_pitch; y+=4) {
            *(uint32_t*)(info->framebuffer_addr + y) = (uint32_t)(-1) << 12;  //ARGB
        }
    }

    target_device_setup();

    UID cpid = ForkCurrentProcess();
    if(cpid == 0) {
        load_elf("sys_init.elf");
    }

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
