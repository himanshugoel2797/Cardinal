/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>

#include "ihda.h"


static uint64_t PCI_BAR;
static uint32_t isPCI_BAR_IO;

static uint32_t *corb_buffer;
static uint64_t *rirb_buffer;
static uint16_t write_pos;
static uint16_t response_pos;
static uint16_t corb_buf_sz;

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
IHDA_Reset(void) {

    Write16(IHDA_STATESTS_REG, (1 << 15) - 1);

    //Reset
    Write32(IHDA_GCTL_REG, 0);  //Clear the reset bit
    while((Read32(IHDA_GCTL_REG) & 1) != 0)
        ;

    Write32(IHDA_GCTL_REG, IHDA_GCTL_RESET);
    while((Read32(IHDA_GCTL_REG) & 1) == 0)
        ;

    while(Read16(IHDA_STATESTS_REG) == 0)
        ;
}

uint16_t
IHDA_AllocateMaxSizeBuffer(uint8_t reg, uint8_t *ret_val) {

    if(reg & IHDA_1024B_SUP_BIT) {
        *ret_val = IHDA_1024B_VAL;
        return 256;
    } else if(reg & IHDA_64B_SUP_BIT) {
        *ret_val = IHDA_64B_VAL;
        return 16;
    }

    *ret_val = IHDA_8B_VAL;
    return 2;
}

void
IHDA_WriteCommand(uint32_t codec, uint32_t node, IHDA_CORB_CMDs cmd, uint8_t data) {

    while(Read16(IHDA_CORB_READ_REG) != Read16(IHDA_CORB_WRITE_REG))
        ;

    write_pos = (write_pos + 1) % corb_buf_sz;
    corb_buffer[write_pos] = IHDA_CORB_VERB(codec, node, cmd, data);

    Write16(IHDA_CORB_WRITE_REG, write_pos);
}

uint64_t
IHDA_ReadResponse(void) {

    uint16_t rirb_pos = Read16(IHDA_RIRB_WRITE_REG);
    uint64_t retVal = rirb_buffer[rirb_pos];
    rirb_buffer[rirb_pos] = 0;
    return retVal;
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
    for(int i = 0; i < device.BarCount; i++) {

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

        if(vmin == IHDA_MINOR_VER && vmaj == IHDA_MAJOR_VER && outpay == IHDA_OUTPAY_RESETVAL) {
            break;  //Valid BAR has been found.
        }

        if(i + 1 == device.BarCount) {
            //No BAR found that matches
            return -1;
        }
    }

    PCI_EnableBusMaster(&device);
    IHDA_Reset();




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

    corb_buffer = (uint32_t*)dma_buffer;
    rirb_buffer = (uint64_t*)(dma_buffer + 1024);
    write_pos = 0;

    uint64_t corb_base_addr = dma_phys_addr;
    uint64_t rirb_base_addr = dma_phys_addr + 1024; //Always allocate 1024 bytes since we need to do alignment.

    while(Read8(IHDA_CORB_CTRL_REG) & IHDA_CORB_RUN)
        Write8(IHDA_CORB_CTRL_REG, 0);

    while(Read8(IHDA_RIRB_CTRL_REG) & IHDA_RIRB_RUN)
        Write8(IHDA_RIRB_CTRL_REG, 0);

    //Determine the maximum size of the corb buffer available
    uint8_t corb_sz = 0;
    corb_buf_sz = IHDA_AllocateMaxSizeBuffer(Read8(IHDA_CORB_SZ_REG), &corb_sz);
    Write8(IHDA_CORB_SZ_REG, (Read8(IHDA_CORB_SZ_REG) & ~3) | corb_sz);

    //Determine the maximum size of the rirb buffer avaiable
    uint8_t rirb_sz = 0;
    int rirb_buf_sz = IHDA_AllocateMaxSizeBuffer(Read8(IHDA_RIRB_SZ_REG), &rirb_sz);
    Write8(IHDA_RIRB_SZ_REG, (Read8(IHDA_RIRB_SZ_REG) & ~3) | rirb_sz);

    //Setup the CORB base address
    Write32(IHDA_CORB_LO_REG, (uint32_t)corb_base_addr);
    Write32(IHDA_CORB_HI_REG, (uint32_t)(corb_base_addr >> 32));

    //Setup the RIRB base address
    Write32(IHDA_RIRB_LO_REG, (uint32_t)rirb_base_addr);
    Write32(IHDA_RIRB_HI_REG, (uint32_t)(rirb_base_addr >> 32));


    //Setup the CORB pointers
    {

        Write16(IHDA_CORB_WRITE_REG, 0);

        //Reset the read corb register
        while(!(Read16(IHDA_CORB_READ_REG) & (1 << 15)))
            Write16(IHDA_CORB_READ_REG, 1 << 15);

        Write16(IHDA_CORB_READ_REG, 0);
        while(Read16(IHDA_CORB_READ_REG) & (1 << 15))
            ;

        while(!(Read8(IHDA_CORB_CTRL_REG) & IHDA_CORB_RUN))
            Write8(IHDA_CORB_CTRL_REG, IHDA_CORB_RUN);

    }

    //Setup the RIRB pointers
    {
        //Reset the write rirb register
        Write16(IHDA_RIRB_WRITE_REG, 1 << 15);

        Write16(IHDA_RIRB_INTCNT_REG, 1);

        while(!(Read8(IHDA_RIRB_CTRL_REG) & IHDA_RIRB_RUN))
            Write8(IHDA_RIRB_CTRL_REG, IHDA_RIRB_RUN);
    }

    IHDA_WriteCommand(0, 0, CMD_GetParameter, 4);

    while(Read16(IHDA_CORB_READ_REG) == 0)
        ;

//    __asm__("hlt" :: "a"(IHDA_ReadResponse()));



    while(1) {

        //Check for messages


        //Receive transmit request through writes


        //Respond to reads with receive buffers


    }

    return 0;
}