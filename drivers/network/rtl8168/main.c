/**
 * Copyright (c) 2017 Himanshu Goel
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "io.h"

uint8_t device_MAC[6];

void
ReadMAC(void) {
    for(int i = 0; i < 6; i++){
        device_MAX[i] = Read8(i);
    }
}

void
Reset(void) {
    Write8(0x37, 0x10);
    while(Read8(0x37) & 0x10);

    ReadMAC();


}

/*
Network stack design: Runs under net_srv. Driver registers to net_srv, providing its MAC address as well as link information, feature information and send/receive buffer keys.
Driver reads its assigned device file for a bitmask of features that the network server would like for it to enable.
Other than that, driver responds to status requests as well as reset requests.
*/

int main(int argc, char *argv[]) {

    R01_GetIOPrivileges();

    PCI_Device device;
    PCI_GetPCIDevice(argv[1], &device);


    uint64_t PCI_BAR = PCI_GetBAR(&device, 0);
    bool isPCI_BAR_IO = PCI_IsIOSpaceBAR(&device, 0);

    if(PCI_BAR == 0) {
        PCI_BAR = PCI_GetBAR(&device, 1);
        isPCI_BAR_IO = PCI_IsIOSpaceBAR(&device, 1);
    }

    if(!isPCI_BAR_IO) {
        //Parse BAR0's key from the args
        Key_t bar0_key;
        char bar0_key_buf[KEY_STR_LEN];

        sscanf(argv[2], "B0:%s", bar0_key_buf);
        StringToKey(bar0_key_buf, &bar0_key);

        UserSharedMemoryData data;
        ApplySharedMemoryKey(&bar0_key, &data);

        PCI_BAR = (uint64_t)data.VirtualAddress;
    }

    PCI_EnableBusMaster(&device);
    SetPCIBAR(PCI_BAR, isPCI_BAR_IO);

    //Reset
    Reset();

    /*
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

    uint8_t mac[6];
    for(int i = 0; i < 6; i++)
        mac[i] = Read8(i);


    //Transmit buffer
    uint64_t trn_dma_buffer = 0;
    uint64_t trn_dma_phys_addr = 0;
    MemoryAllocationErrors err = 0;
    err = MMap(&trn_dma_buffer,
               4 * 1024,
               MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
               MMapFlags_DMA,
               CachingModeWriteBack
              );

    R01_GetPhysicalAddress(0, trn_dma_buffer, &trn_dma_phys_addr);
    Write32(0x20, (uint32_t)trn_dma_phys_addr);
    */

    while(1) {

        //Check for messages


        //Receive transmit request through writes


        //Respond to reads with receive buffers


    }

    return 0;
}