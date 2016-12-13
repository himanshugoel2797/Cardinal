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
    KeyMan_Initialize();
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

static int wait_cnt = 0;

typedef struct {
    Message m;
    uint64_t key;
} KeyMessage;

void
idle_thread(void) {

    int c = 0;

    while(1) {

        kfree(kmalloc(512));

        if(GetCurrentProcessUID() % 3 == 0) {
            wait_cnt++;
            __asm__("cli");
            SleepThreadForMessage(GetCurrentProcessUID(), MessageWaitType_Any, 0);
            __asm__("sti");
            KeyMessage *m2 = kmalloc(512);
            __asm__("cli");
            GetMessageFrom((Message*)m2, 0, 0);
            __asm__("sti");

            uint64_t vAddr = 0;
            MemoryAllocationFlags flags = 0;
            CachingMode cacheMode;
            uint64_t len = 0;

            __asm__("cli");
            ApplySharedMemoryKey(GetCurrentProcessUID(),
                                 m2->key,
                                 &vAddr,
                                 &flags,
                                 &cacheMode,
                                 &len);
            __asm__("sti");

            __asm__("cli");
            UnmapPage(GetActiveVirtualMemoryInstance(),
                      vAddr,
                      len);
            __asm__("sti");

            kfree(m2);
        }

        if(GetCurrentProcessUID() % 3 == 1) {
            
            KeyMessage *m = kmalloc(512);

            uint64_t virtAddr = 0;

            __asm__("cli");
            AllocateSharedMemory(GetCurrentProcessUID(),
                                 4096,
                                 CachingModeWriteBack,
                                 MemoryAllocationType_MMap,
                                 MemoryAllocationFlags_Present | MemoryAllocationFlags_Write | MemoryAllocationFlags_Kernel,
                                 &virtAddr);
            __asm__("sti");

            __asm__("cli");
            GetSharedMemoryKey(GetCurrentProcessUID(),
                                virtAddr,
                                4096,
                                CachingModeWriteBack,
                                MemoryAllocationFlags_Present | MemoryAllocationFlags_Write | MemoryAllocationFlags_Kernel,
                                &m->key);
            __asm__("sti");                           

            __asm__("cli");
            PostMessages(GetCurrentProcessUID() - 1, (Message**)&m, 1);
            __asm__("sti");

            while(1){
            __asm__("cli");
            uint64_t k_cnt = 0;
            GetSharedMemoryKeyUsageCount(m->key, &k_cnt);
            __asm__("sti");

            if(k_cnt != 0)
                break;
            }

            __asm__("cli");
            FreeSharedMemoryKey(GetCurrentProcessUID(), m->key);
            __asm__("sti");

            __asm__("cli");
            UnmapPage(GetActiveVirtualMemoryInstance(),
                      virtAddr,
                      4096);
            __asm__("sti");

            kfree(m);
        }

        c++;

        __asm__("cli");
        if(c == 1)ScheduleProcessForTermination(GetCurrentProcessUID(), 0);
        __asm__("sti");
        if(c == 1)while(1);        
    }
}

void
kernel_main(void) {

    //Initialize process manager, setup timers, get threading up and running
    // Enumerate and initialize drivers
    // Load UI elf from disk
    // Switch to usermode
    // Execute UI

    //Seed the rng with the timer value
    seed(GetTimerValue());

    InterruptMan_Initialize();

    SyscallMan_Initialize();
    Syscall_Initialize();
    DeviceManager_Initialize();
    //smp_unlock_cores();
    SetupPreemption();
    target_device_setup();

    for(int i = 0; i < 500; i++){

        UID cpid = 0;
        if(CreateProcess(ROOT_PID, 0, &cpid) != ProcessErrors_None)
            HaltProcessor();

        CreateThread(cpid, ThreadPermissionLevel_Kernel, (ThreadEntryPoint)idle_thread, NULL);
        StartProcess(cpid);
    }
    while(1)
        WakeReadyThreads();

    //load_exec(cpid, "userboot.bin");

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
