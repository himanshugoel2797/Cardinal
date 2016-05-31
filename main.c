#include "main.h"
#include "types.h"
#include "multiboot.h"
#include "kmalloc/kmalloc.h"
#include "managers.h"
#include "memory.h"
#include "syscall.h"

void
kernel_stall(void)
{
    __asm__ ("cli\n\thlt");
}

void
kernel_main_init(void) {
    MemoryAllocationsMap *allocMap = bootstrap_malloc(sizeof(MemoryAllocationsMap));
    kmalloc_init (allocMap);
    ProcessSys_Initialize(allocMap);
    Thread_Initialize();
    CreateThread(0, kernel_main);
    CreateThread(0, kernel_stall);
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
    target_device_setup();
    
    __asm__ ("hlt");

    while(1);
    //The kernel is ready to take in the new cores, bring them up
}

void
smp_core_main(int coreID,
              int (*getCoreData)(void)) {
    coreID = 0;
    getCoreData = NULL;
    //RegisterCore(coreID, getCoreData);
    //Start the local timer and set it to call the thread switch handler
}