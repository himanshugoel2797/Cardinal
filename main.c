#include "main.h"
#include "types.h"
#include "multiboot.h"
#include "kmalloc/kmalloc.h"
#include "managers.h"

void
kernel_main_init(void) {
    kmalloc_init ();
    ProcessSys_Initialize();
    Thread_Initialize();
}

void
kernel_main(void) {

    //Initialize process manager, setup timers, get threading up and running
    // Enumerate and initialize drivers
    // Load UI elf from disk
    // Switch to usermode
    // Execute UI

    //The kernel is ready to take in the new cores, bring them up
    smp_unlock_cores();
}

void
smp_core_main(int coreID,
              int (*getCoreData)(void)) {
    RegisterCore(coreID, getCoreData);
    while(1) {

    }
}