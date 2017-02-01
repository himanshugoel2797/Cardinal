/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/cardinal_types.h>
#include <cardinal/syscall_property.h>
#include <cardinal/driver_utils.h>
#include <cardinal/io/server.h>
#include <cardinal/shared_memory.h>
#include <cardinal/namespace/server.h>
#include <cardinal/proc/server.h>
#include <cardinal/ipc.h>


#include <string.h>
#include <stdio.h>

#include "pci.h"

int main() {

    //Request IO privileges
    R01_GetIOPrivileges();

    //Get the boot information, retrieve the ACPI table base address
    //Find the MCFG table
    //Get the ECAM addresses

    //Open the list describing which drivers support which hardware
    UID dst_pid = 0;
    uint64_t error = 0;
    uint32_t tmp_key = 0;
    RetrieveNamespace("initrd", &tmp_key);
    while(!IsNamespaceRetrieved(tmp_key, &dst_pid, &error));

    uint8_t access_key[KEY_BYTES];
    uint64_t fd = 0;
    IO_Open(":pci_devices.txt", FileSystemOpFlag_Read, 0, access_key, dst_pid, &fd);

    FileSystemDirectoryEntry file_info;
    IO_GetFileProperties(":pci_devices.txt", &file_info, dst_pid);

    uint64_t len = file_info.Length;
    uint64_t addr = 0;
    Key_t read_key;
    Key_t write_key;

    IO_AllocateBuffer(&len, &addr, &read_key, &write_key);
    IO_Read(fd, 0, read_key, file_info.Length, dst_pid);

    uint32_t str_len = file_info.Length;
    char *file_ptr = (char*)addr;
    char *file_end = file_ptr + str_len;

    //Enumerate PCI devices
    uint32_t bus = 0;
    uint32_t device = 0;

    int dev_index = -1;

    while(1) {
        PCI_GetNextDevice(&bus, &device);
        dev_index++;

        if(bus != -1 && device != -1) {

            uint32_t funcs = PCI_GetFuncCount(bus, device);
            PCI_Device devInfo;


            for(uint32_t f = 0; f < funcs; f++) {
                PCI_GetPCIDevice(bus, device, f, &devInfo);


                char tStr[512];
                sprintf(tStr, "B:%x D:%x F:%x DID:%x VID:%x CLS:%x SCLS:%x PIF:%x\r\n", bus, device, f, devInfo.DeviceID,
                        devInfo.VendorID,
                        devInfo.ClassCode,
                        devInfo.SubClassCode,
                        devInfo.ProgIF);
                int tStr_i = 0;
                while(tStr[tStr_i]) {
                    outb(0x3f8, tStr[tStr_i++]);
                }

                //Check the device information file for this device
                char *cur_loc = file_ptr;
                while(cur_loc != NULL && cur_loc < file_end) {

                    uint32_t vendor_id = 0;
                    uint32_t device_id = 0;
                    uint32_t class_code = 0;
                    uint32_t subclass_code = 0;
                    uint32_t prog_if = 0;

                    char driver_file[256];
                    memset(driver_file, 0, 256);
                    driver_file[0] = ':';

                    sscanf(cur_loc, "%s |%x|%x|%x|%x|%x\n", driver_file + 1,
                           &vendor_id,
                           &device_id,
                           &class_code,
                           &subclass_code,
                           &prog_if);

                    cur_loc = strchr(cur_loc, '\n') + 1;

                    int vendor_m = (vendor_id == devInfo.VendorID || vendor_id == 0xFFFF);
                    int dev_m = (device_id == devInfo.DeviceID || device_id == 0xFFFF);
                    int class_m = (class_code == devInfo.ClassCode || class_code == 0xFFFF);
                    int subclass_m = (subclass_code == devInfo.SubClassCode || subclass_code == 0xFFFF);
                    int prog_if_m = (prog_if == devInfo.ProgIF || prog_if == 0xFFFF);


                    if(vendor_m && dev_m && class_m && subclass_m && prog_if_m) {

                        char devStr[512];
                        sprintf(devStr, "B:%d D:%d F:%d DID:%x VID:%x", bus, device, f, device_id, vendor_id);

                        uint64_t bar_vals[6];
                        uint64_t bar_sz[6];
                        Key_t bar_keys[6];

                        char bar_keys_buf[6][KEY_STR_LEN];

                        for(int i = 0; i < devInfo.BarCount; i++) {
                            bar_vals[i] = PCI_GetBAR(&devInfo, i);

                            uint64_t sz = PCI_GetBARSize(&devInfo, i);
                            bar_sz[i] = sz;

                            if(bar_vals[i] != 0 && !PCI_IsIOSpaceBAR(&devInfo, i)) {

                                //Setup the mapping
                                R0_AllocateSharedMemory(sz,
                                                        CachingModeUncachable,
                                                        MemoryAllocationType_MMap |
                                                        MemoryAllocationType_Phys,
                                                        MemoryAllocationFlags_Write |
                                                        MemoryAllocationFlags_Read |
                                                        MemoryAllocationFlags_User |
                                                        MemoryAllocationFlags_Present,
                                                        bar_vals[i],
                                                        &bar_vals[i]);

                                GetSharedMemoryKey(bar_vals[i],
                                                   sz,
                                                   CachingModeUncachable,
                                                   MemoryAllocationFlags_Write |
                                                   MemoryAllocationFlags_Read |
                                                   MemoryAllocationFlags_User |
                                                   MemoryAllocationFlags_Present,
                                                   &bar_keys[i]);

                                KeyToString(bar_keys[i], bar_keys_buf[i]);
                            } else {
                                bar_vals[i] = 0;
                            }
                        }

                        char bars[512];
                        sprintf(bars, "B0:%s B1:%s B2:%s B3:%s B4:%s B5:%s",
                                bar_keys_buf[0],
                                bar_keys_buf[1],
                                bar_keys_buf[2],
                                bar_keys_buf[3],
                                bar_keys_buf[4],
                                bar_keys_buf[5]);

                        char *argv[] = { driver_file + 1, devStr, bars };

                        int devStr_i = 0;
                        while(driver_file[devStr_i]) {
                            outb(0x3f8, driver_file[devStr_i++]);
                        }

                        UID pid = 0;
                        if(StartProcess(driver_file,
                                        argv,
                                        3,
                                        dst_pid,
                                        access_key,
                                        1,
                                        &pid) != 0)
                            __asm__("hlt");
                        break;
                    }
                }

                //Load the associated drivers, passing a key to the appropriate shared memory block


                //Start the driver


            }

        } else
            break;

        device++;
    }

    CREATE_NEW_MESSAGE_PTR(msg);
    msg->MsgType = CardinalMsgType_Notification;
    msg->MsgID = 0;
    PostIPCMessage(2 /*userboot PID*/, msg);

    //while(1);
}