#include "main.h"
#include "types.h"
#include "multiboot.h"
#include "kmalloc/kmalloc.h"
#include "managers.h"
#include "memory.h"
#include "syscall.h"
#include "initrd.h"
#include "elf.h"

void
sleep_kernel(void) {
    __asm__("cli\n\thlt" :: "a"(0xDEADB33F));
}

void
hlt_kernel(void) {
    while(1);
}

void
kernel_main_init(void) {
    MemoryAllocationsMap *allocMap = bootstrap_malloc(sizeof(MemoryAllocationsMap));

    kmalloc_init (allocMap);
    ProcessSys_Initialize(allocMap);
    Thread_Initialize();
    RegisterCore(0, NULL);
    CreateThread(0, kernel_main);
    smp_unlock_cores();
    CoreUpdate(0);  //BSP is core 0
}

void
kernel_main(void) {
    //Initialize process manager, setup timers, get threading up and running
    // Enumerate and initialize drivers
    // Load UI elf from disk
    // Switch to usermode
    // Execute UI

    Syscall_Initialize();
    DeviceManager_Initialize();
    setup_preemption();
    target_device_setup();

    void *elf_loc = NULL;
    uint64_t elf_size = 0;
    MemoryAllocationsMap *m;


    Initrd_GetFile("build/test.elf", &elf_loc, &elf_size);
    LoadElf(elf_loc, elf_size, ElfLimitations_64Bit | ElfLimitations_LSB, GetActiveVirtualMemoryInstance(), &m, NULL);

    __asm__ volatile("push $0x4000b0\n\tretq");
    while(1)__asm__ ("cli\n\thlt");

    //The kernel is ready to take in the new cores, bring them up
    FreeThread(GetCurrentThreadUID());
}

void
smp_main(void) {
    setup_preemption();

    while(1);
}

void
smp_core_main(int coreID,
              int (*getCoreData)(void)) {
    coreID = 0;
    getCoreData = NULL;
    while(1);
    //Syscall_Initialize();
    RegisterCore(coreID, getCoreData);
    CreateThread(0, smp_main);
    CoreUpdate(coreID);
    //Start the local timer and set it to call the thread switch handler
}