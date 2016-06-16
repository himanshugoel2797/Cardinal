#include "main.h"
#include "types.h"
#include "multiboot.h"
#include "kmalloc/kmalloc.h"
#include "managers.h"
#include "memory.h"
#include "syscall.h"
#include "initrd.h"
#include "elf.h"
#include "synchronization.h"

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
    MemoryAllocationsMap *allocMap = bootstrap_malloc(sizeof(MemoryAllocationsMap));
    allocMap->next = NULL;

    kmalloc_init (allocMap);
    ProcessSys_Initialize(allocMap);
    Thread_Initialize();
    RegisterCore(0, NULL);
    CreateThread(0, kernel_main);
    CoreUpdate();  //BSP is core 0
}

void
load_elf(void) {
    void *elf_loc = NULL;
    uint64_t elf_size = 0;

    ProcessInformation p_info;
    GetProcessInformation(GetCurrentProcessUID(), &p_info);

    MemoryAllocationsMap *m = p_info.AllocationMap;
    ElfInformation elf_info;
    Initrd_GetFile("build/test.elf", &elf_loc, &elf_size);
    LoadElf(elf_loc, elf_size, ElfLimitations_64Bit | ElfLimitations_LSB, GetActiveVirtualMemoryInstance(), &m, &elf_info);

    SwitchToUserMode((uint64_t)elf_info.entry_point, (uint64_t)GetThreadUserStack(GetCurrentThreadUID()));
    while(1) {
        YieldThread();
        __asm__("cli\n\thlt");
    }
}

int coreCount = 0;

void
kernel_main(void) {
    //Initialize process manager, setup timers, get threading up and running
    // Enumerate and initialize drivers
    // Load UI elf from disk
    // Switch to usermode
    // Execute UI

    coreCount++;
    Syscall_Initialize();
    DeviceManager_Initialize();
    setup_preemption();
    target_device_setup();
    smp_lock = CreateSpinlock();
    //smp_unlock_cores();

    //while(coreCount != GetCoreCount());

    ProcessInformation p_info;
    GetProcessInformation(0, &p_info);
    ProcessInformation *elf_proc;
    ForkProcess(&p_info, &elf_proc);
    if(!CreateThread(elf_proc->ID, load_elf))__asm__("cli\n\thlt");
    //CreateThread(0, load_elf);

    FreeThread(GetCurrentThreadUID());
}


void
smp_main(void) {
    LockSpinlock(smp_lock);
    setup_preemption();
    coreCount++;
    UnlockSpinlock(smp_lock);
    CreateThread(0, hlt_kernel);
    CreateThread(0, hlt2_kernel);
    FreeThread(GetCurrentThreadUID());
}

void
smp_core_main(int coreID,
              int (*getCoreData)(void)) {
    getCoreData = NULL;
    Syscall_Initialize();
    RegisterCore(coreID, getCoreData);
    CreateThread(0, smp_main);
    CoreUpdate();
    //Start the local timer and set it to call the thread switch handler
}