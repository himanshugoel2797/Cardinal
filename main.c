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

    uint64_t orig_exec_size = exec_size;
    exec_size += EXEC_ENTRY_POINT % PAGE_SIZE;
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
    //smp_unlock_cores();
    SetupPreemption();
    target_device_setup();

    UID cpid = 0;
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    load_exec(cpid, "userboot.bin");

    while(1) YieldThread();
}

void
idle_main(void) {
    while(1);
}

void
smp_core_main(int coreID,
              int (*getCoreData)(void)) {


    //Expose additional cores as a service
    Syscall_Initialize();

    RegisterCore(coreID, getCoreData);

    UID cpid = 0;
    if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
        HaltProcessor();

    CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_main, NULL);
    StartProcess(cpid);

    SetupPreemption();
    CoreUpdate();
    while(1);
    //Start the local timer and set it to call the thread switch handler
}
