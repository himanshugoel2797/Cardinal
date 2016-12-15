#include <cardinal/cardinal_types.h>
#include <cardinal/syscall_property.h>
#include <cardinal/driver_utils.h>
#include <cardinal/io/server.h>
#include <cardinal/namespace/server.h>

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

    //Enumerate PCI devices
	uint32_t bus = 0;
	uint32_t device = 0;

	while(1){
		PCI_GetNextDevice(&bus, &device);
		
		if(bus != -1 && device != -1){
			
			uint32_t funcs = PCI_GetFuncCount(bus, device);
			PCI_Device devInfo;

			for(uint32_t f = 0; f < funcs; f++) {
				PCI_GetPCIDevice(bus, device, f, &devInfo);

				//Check the device information file for this device
				

    			//Load the associated drivers, passing a key to the appropriate shared memory block


    			//Start the driver
				

			}

		}else
			break;
	}
}