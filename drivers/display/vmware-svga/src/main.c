#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>

#include "svga.h"

int main(int argc, char *argv[]){
    R01_GetIOPrivileges();

    SVGA_Context ctxt;
    PCI_GetPCIDevice(argv[1], &ctxt.device);

    ctxt.iobase = PCI_GetBAR(&ctxt.device, 0);

    //Parse BAR0's key from the args
    uint64_t bar0_key = 0;
    uint64_t bar1_key = 0;
    uint64_t bar2_key = 0;

    sscanf(argv[2], "B0:%llx B1:%llx B2:%llx", &bar0_key, &bar1_key, &bar2_key);

    /*
    UserSharedMemoryData data;
    ApplySharedMemoryKey(bar0_key, &data);
    ctxt.iobase = (uint64_t)data.VirtualAddress;
	*/

    UserSharedMemoryData data;
    ApplySharedMemoryKey(bar1_key, &data);
    ctxt.fbmem = (uint64_t)data.VirtualAddress;

    ApplySharedMemoryKey(bar2_key, &data);
    ctxt.fifomem = (uint64_t)data.VirtualAddress;


    PCI_EnableBusMaster(&ctxt.device);

    SVGA_Init(&ctxt);



    while(1);

    //Power On
    Write8(0x52, 0x0);

    //Reset
    Write8(0x37, 0x10);
    while(Read8(0x37) & 0x10);

    //Setup receive buffer
    //Allocate 32K + 16 + 1500 bytes for buffer
    uint64_t rcv_dma_buffer = 0;
    uint64_t rcv_dma_phys_addr = 0;
    MMap(&rcv_dma_buffer,
         36 * 1024,
         MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
         MMapFlags_DMA,
         CachingModeWriteBack
        );

    R01_GetPhysicalAddress(0, rcv_dma_buffer, &rcv_dma_phys_addr);
    Write32(0x30, (uint32_t)rcv_dma_phys_addr);

    //Configure the receive buffer to receive all broadcast packets, but no promiscious mode
    //Buffer size = 32K + 16 bytes + 1500 bytes
    Write32(0x44, (2 << 11) | 0xE | (1 << 7));
    //Write32(0x40, 1 << 16); Disables appending a CRC

    //Enable receiver and transmit, poll
    Write8(0x37, 0x0C);


    while(1) {

        //Check for messages


        //Receive transmit request through writes


        //Respond to reads with receive buffers


    }
}