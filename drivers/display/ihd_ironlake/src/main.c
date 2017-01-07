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
    ApplySharedMemoryKey(bar0_key, &data);
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