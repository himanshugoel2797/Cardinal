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
load_exec(const char *exec) {
    void *exec_loc = NULL;
    uint64_t exec_size = 0;

    Initrd_GetFile(exec, &exec_loc, &exec_size);

    void* sp = GetThreadUserStack(GetCurrentThreadUID());
    SwitchToUserMode(EXEC_ENTRY_POINT, (uint64_t)sp);
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

    {
        CardinalBootInfo *info = GetBootInfo();
        for(uint32_t y = 0; y < info->FramebufferHeight * info->FramebufferPitch; y+=4) {
            *(uint32_t*)(info->FramebufferAddress + y) = (uint32_t)(-1) << 0;  //ARGB
        }
    }
    SetupPreemption();

    {
        CardinalBootInfo *info = GetBootInfo();
        for(uint32_t y = 0; y < info->FramebufferHeight * info->FramebufferPitch; y+=4) {
            *(uint32_t*)(info->FramebufferAddress + y) = (uint32_t)(-1) << 8;  //ARGB
        }
    }

    target_device_setup();

    UID cpid = ForkCurrentProcess();

    {
        CardinalBootInfo *info = GetBootInfo();
        for(uint32_t y = 0; y < info->FramebufferHeight * info->FramebufferPitch; y+=4) {
            *(uint32_t*)(info->FramebufferAddress + y) = (uint32_t)(-1) << 16;  //ARGB
        }
    }

    if(cpid == 0) {
        load_exec("userboot.bin");
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
