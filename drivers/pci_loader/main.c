#include <cardinal/cardinal_types.h>
#include <cardinal/syscall_property.h>
#include <cardinal/driver_utils.h>
#include <cardinal/io/server.h>
#include <cardinal/namespace/server.h>
#include <cardinal/proc/server.h>

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
    uint64_t read_key = 0;
    uint64_t write_key = 0;

    IO_AllocateBuffer(&len, &addr, &read_key, &write_key);
    IO_Read(fd, 0, read_key, file_info.Length, dst_pid);

	uint32_t str_len = file_info.Length;
    char *file_ptr = (char*)addr;
    char *file_end = file_ptr + str_len;

    //Enumerate PCI devices
	uint32_t bus = 0;
	uint32_t device = 0;

	int dev_index = -1;

	while(1){
		PCI_GetNextDevice(&bus, &device);
		dev_index++;

		if(bus != -1 && device != -1){
			
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
				while(tStr[tStr_i]){
					outb(0x3f8, tStr[tStr_i++]);
				}

				//Check the device information file for this device
				char *cur_loc = file_ptr;
				while(cur_loc < file_end) {

					uint32_t vendor_id = 0;
					uint32_t device_id = 0;
					uint32_t class_code = 0;
					uint32_t subclass_code = 0;
					uint32_t prog_if = 0;

					char driver_file[256];
					driver_file[0] = ':';

					sscanf(cur_loc, "%s |%x|%x|%x|%x|%x\n", driver_file + 1, 
												  		   &vendor_id, 
												  		   &device_id, 
												  		   &class_code, 
												  		   &subclass_code, 
												  		   &prog_if);

					cur_loc = strchr(cur_loc, '\n') + 1;


					if(vendor_id == devInfo.VendorID && 
						device_id == devInfo.DeviceID &&
						class_code == devInfo.ClassCode &&
						subclass_code == devInfo.SubClassCode) {


						if(prog_if != -1 && prog_if != devInfo.ProgIF) {
							continue;
						}

						char devStr[256];
						sprintf(devStr, "B:%d D:%d F:%d DID:%x VID:%x", bus, device, f, device_id, vendor_id);

						char *argv[] = { driver_file + 1, devStr };

						UID pid = 0;
						StartProcess(driver_file,
									 argv,
									 1,
									 dst_pid,
									 access_key,
									 1,
									 &pid);
						break;
					}
				}

    			//Load the associated drivers, passing a key to the appropriate shared memory block


    			//Start the driver
				

			}

		}else
			break;

		device++;
	}
}