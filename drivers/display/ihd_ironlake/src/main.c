#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>

#include "ihd_context.h"

int main(int argc, char *argv[]) {
    R01_GetIOPrivileges();

    IHD_Context ctxt;
    PCI_GetPCIDevice(argv[1], &ctxt.device);

    //Parse BAR0's key from the args
    uint64_t bar0_key = 0;
    uint64_t bar1_key = 0;
    uint64_t bar2_key = 0;

    sscanf(argv[2], "B0:%llx B1:%llx B2:%llx", &bar0_key, &bar1_key, &bar2_key);

    UserSharedMemoryData data;
    ApplySharedMemoryKey(bar1_key, &data);
    ctxt.iobase = data.VirtualAddress;

    PCI_EnableBusMaster(&ctxt.device);

    uint64_t bounce_buffer = 0;
    uint64_t bounce_phys_addr = 0;
    MMap(&bounce_buffer,
         1024 * 1024,
         MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
         MMapFlags_DMA,
         CachingModeWriteBack
        );
    R01_GetPhysicalAddress(0, bounce_buffer, &bounce_phys_addr);

    IHD_Init(&ctxt);

    while(1) {

        //Check for messages


        //Receive transmit request through writes


        //Respond to reads with receive buffers


    }
}