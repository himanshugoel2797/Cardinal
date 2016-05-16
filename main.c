#include "main.h"
#include "types.h"
#include "multiboot.h"
#include "kmalloc/kmalloc.h"
#include "managers.h"

void
kernel_main(void) {
    kmalloc_init ();
    ProcessSys_Initialize();
    //Initialize process manager, setup timers, get threading up and running
    // Enumerate and initialize drivers
    // Load UI elf from disk
    // Switch to usermode
    // Execute UI
}
