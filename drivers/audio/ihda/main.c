#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>

#include "ihda.h"


static uint64_t PCI_BAR;
static uint32_t isPCI_BAR_IO;

void
Write8(uint32_t off, uint8_t val) {
    if(isPCI_BAR_IO) {
        outb(PCI_BAR + off, val);
    } else {
        *(uint8_t*)(PCI_BAR + off) = val;
    }
}

uint8_t
Read8(uint32_t off) {
    if(isPCI_BAR_IO) {
        return inb(PCI_BAR + off);
    } else {
        return *(uint8_t*)(PCI_BAR + off);
    }
}

void
Write16(uint32_t off, uint16_t val) {
    if(isPCI_BAR_IO) {
        outw(PCI_BAR + off, val);
    } else {
        *(uint16_t*)(PCI_BAR + off) = val;
    }
}

uint16_t
Read16(uint32_t off) {
    if(isPCI_BAR_IO) {
        return inw(PCI_BAR + off);
    } else {
        return *(uint16_t*)(PCI_BAR + off);
    }
}

void
Write32(uint32_t off, uint32_t val) {
    if(isPCI_BAR_IO) {
        outl(PCI_BAR + off, val);
    } else {
        *(uint32_t*)(PCI_BAR + off) = val;
    }
}

uint32_t
Read32(uint32_t off) {
    if(isPCI_BAR_IO) {
        return inl(PCI_BAR + off);
    } else {
        return *(uint32_t*)(PCI_BAR + off);
    }
}

void
IHDA_Reset(void){

    //Reset
    Write32(IHDA_GCTL_REG, 0);  //Clear the reset bit
    while((Read32(IHDA_GCTL_REG) & 1) != 0)
        ;
    Write32(IHDA_GCTL_REG, IHDA_GCTL_RESET);
    while((Read32(IHDA_GCTL_REG) & 1) == 0)
        ;
}

int
IHDA_AllocateMaxSizeBuffer(uint8_t reg, uint8_t *ret_val){

    if(reg & IHDA_1024B_SUP_BIT) {
        *ret_val = IHDA_1024B_VAL;
        return 1024;
    }else if(reg & IHDA_64B_SUP_BIT) {
        *ret_val = IHDA_64B_VAL;
        return 64;
    }

    *ret_val = IHDA_8B_VAL;
    return 8;
}

int main(int argc, char *argv[]) {

    R01_GetIOPrivileges();

    PCI_Device device;
    PCI_GetPCIDevice(argv[1], &device);

    //Parse BAR keys from the args
    uint64_t bar_keys[6] = {0};
    sscanf(argv[2], "B0:%llx B1:%llx B2:%llx B3:%llx B4:%llx B5:%llx", &bar_keys[0], 
                                                                       &bar_keys[1], 
                                                                       &bar_keys[2], 
                                                                       &bar_keys[3], 
                                                                       &bar_keys[4], 
                                                                       &bar_keys[5]);

    //Attempt to determine which BAR is the one with the hda registers.
    for(int i = 0; i < device.BarCount; i++){
        
        PCI_BAR = PCI_GetBAR(&device, i);
        isPCI_BAR_IO = PCI_IsIOSpaceBAR(&device, i);

        if(PCI_BAR == 0)
            continue;

        if(!isPCI_BAR_IO) {
            uint64_t bar_key = bar_keys[i];

            UserSharedMemoryData data;
            ApplySharedMemoryKey(bar_key, &data);

            PCI_BAR = (uint64_t)data.VirtualAddress;
        }

        //Check the version registers
    
        uint8_t vmin = Read8(IHDA_VMIN_REG);
        uint8_t vmaj = Read8(IHDA_VMAJ_REG);
        uint16_t outpay = Read16(IHDA_OUTPAY_REG);

        if(vmin == IHDA_MINOR_VER && vmaj == IHDA_MAJOR_VER && outpay == IHDA_OUTPAY_RESETVAL)
            break;  //Valid BAR has been found.

        if(i + 1 == device.BarCount){
            //No BAR found that matches
            return -1;
        }
    }

    IHDA_Reset();
    PCI_EnableBusMaster(&device);

    //Determine the maximum size of the corb buffer available
    uint8_t corb_sz = 0;
    int corb_buf_sz = IHDA_AllocateMaxSizeBuffer(Read8(IHDA_CORB_SZ_REG), &corb_sz);
    Write8(IHDA_CORB_SZ_REG, corb_sz);

    //Determine the maximum size of the rirb buffer avaiable
    uint8_t rirb_sz = 0;
    int rirb_buf_sz = IHDA_AllocateMaxSizeBuffer(Read8(IHDA_RIRB_SZ_REG), &rirb_sz);
    Write8(IHDA_RIRB_SZ_REG, rirb_sz);


    //Allocate a buffer for all the dma needs of the device
    uint64_t dma_buffer = 0;
    uint64_t dma_phys_addr = 0;
    MMap(&dma_buffer,
         32 * 1024,
         MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
         MMapFlags_DMA,
         CachingModeWriteBack
        );
    R01_GetPhysicalAddress(0, dma_buffer, &dma_phys_addr);

    uint64_t corb_base_addr = dma_phys_addr;
    uint64_t rirb_base_addr = dma_phys_addr + 1024; //Always allocate 1024 bytes since we need to do alignment.

    //Setup the CORB base address
    Write32(IHDA_CORB_LO_REG, (uint32_t)corb_base_addr);
    Write32(IHDA_CORB_HI_REG, (uint32_t)(corb_base_addr >> 32));

    //Setup the RIRB base address
    Write32(IHDA_RIRB_LO_REG, (uint32_t)rirb_base_addr);
    Write32(IHDA_RIRB_HI_REG, (uint32_t)(rirb_base_addr >> 32));

    //Setup the CORB pointers
    Write16(IHDA_CORB_WRITE_REG, 0);
    Write16(IHDA_CORB_READ_REG, 0);
    Write8(IHDA_CORB_CTRL_REG, IHDA_CORB_RUN);

    //Setup the RIRB pointers
    Write16(IHDA_RIRB_WRITE_REG, 0);
    Write8(IHDA_RIRB_CTRL_REG, IHDA_RIRB_RUN);

    __asm__("hlt");

    while(1) {

        //Check for messages


        //Receive transmit request through writes


        //Respond to reads with receive buffers


    }

    return 0;
}