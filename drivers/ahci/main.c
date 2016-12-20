#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>
#include <string.h>

//Register offsets
#define HBA_GHC 0x4
#define HBA_PI 0xC
#define HBA_PxCMD(x) (0x118 + 0x80 * x)
#define HBA_PxSERR(x) (0x130 + 0x80 * x)
#define HBA_PxTFD(x) (0x120 + 0x80 * x)
#define HBA_PxSSTS(x) (0x128 + 0x80 * x)
#define HBA_PxCLB(x) (0x100 + 0x80 * x)
#define HBA_PxCLBU(x) (0x104 + 0x80 * x)
#define HBA_PxFB(x) (0x108 + 0x80 * x)
#define HBA_PxFBU(x) (0x10C + 0x80 * x)


//Register bits and masks
#define HBA_PxCMD_ST (1 << 0)
#define HBA_PxCMD_CR (1 << 15)
#define HBA_PxCMD_FR (1 << 14)
#define HBA_PxCMD_FRE (1 << 4)

#define HBA_PxTFD_BSY (1 << 7)
#define HBA_PxTFD_DRQ (1 << 3)

#define HBA_PxSSTS_DET_MASK 0xF

//DMA buffer sizes
#define FIS_SIZE 256
#define CMD_BUF_SIZE 1024

static uint64_t PCI_BAR;

static uint32_t activeDevices;

void
Write8(uint32_t off, uint8_t val) {
	*(uint8_t*)(PCI_BAR + off) = val;
}

uint8_t
Read8(uint32_t off) {
	return *(uint8_t*)(PCI_BAR + off);
}

void
Write16(uint32_t off, uint16_t val) {
	*(uint16_t*)(PCI_BAR + off) = val;
}

uint16_t
Read16(uint32_t off) {
	return *(uint16_t*)(PCI_BAR + off);
}

void
Write32(uint32_t off, uint32_t val) {
	*(uint32_t*)(PCI_BAR + off) = val;
}

uint32_t
Read32(uint32_t off) {
	return *(uint32_t*)(PCI_BAR + off);
}

void
ResetHBA(void) {
	//Start the reset
	Write32(HBA_GHC, 1);

	//Wait for the HBA to be done reseting
	while(Read32(HBA_GHC) & 1)
		;
}

void
ReportAHCIAwareness(void) {
	Write32(HBA_GHC, (1 << 31));
}

void
InitializePort(int index, uint32_t dma_base) {
	uint32_t cmd = Read32(HBA_PxCMD(index));

	//Ensure the device is idle before starting initialization
	if(cmd & (HBA_PxCMD_FRE | HBA_PxCMD_FR | HBA_PxCMD_CR | HBA_PxCMD_ST) != 0) {
		//Attempt to clear the offending bits

		cmd &= ~(HBA_PxCMD_ST | HBA_PxCMD_FRE);
		
		for(int i = 0; i < 5; i++) {
			SleepThread(500 * 1000 * 1000);	//Sleep 500ms
			
			if((Read32(HBA_PxCMD(index)) & (HBA_PxCMD_FR | HBA_PxCMD_CR)) == 0) {
				break;
			}
		}

		//Failed to bring the device into an idle state, can't continue with init
		if((Read32(HBA_PxCMD(index)) & (HBA_PxCMD_FR | HBA_PxCMD_CR)) != 0) {
			return;
		}

	}

	//Command buffer is 1024 bytes
	Write32(HBA_PxCLB(index), dma_base + (index * CMD_BUF_SIZE));
	Write32(HBA_PxCLBU(index), 0);

	//FIS buffer is 256 bytes
	Write32(HBA_PxFB(index), dma_base + 32 * CMD_BUF_SIZE + (index * FIS_SIZE));
	Write32(HBA_PxFBU(index), 0);

	//Enable receiving FIS's
	Write32(HBA_PxCMD(index), Read32(HBA_PxCMD(index)) | HBA_PxCMD_FRE);

	//Clear the SERR register by writing ones to all implemented bits
	Write32(HBA_PxSERR(index), 0x07FF0F03);

	uint32_t tfd = Read32(HBA_PxTFD(index));

	//Ensure that the device is functional before setting the bit in the active mask
	//and enabling the HBA 

	if(tfd & (HBA_PxTFD_BSY | HBA_PxTFD_DRQ) != 0) {
		return;	//The device is not in functioning order, TODO we might want to report this
	}

	uint32_t ssts = Read32(HBA_PxSSTS(index));

	if((ssts & HBA_PxSSTS_DET_MASK) != 3) {
		return;	//TODO report this, by writing to a virtual log file or something
	}

	//The device is functional, mark it as so and start it up
	activeDevices |= (1 << index);

	Write32(HBA_PxCMD(index), Read32(HBA_PxCMD(index)) | HBA_PxCMD_ST);
}

int main(int argc, char *argv[]) {

	R01_GetIOPrivileges();	

	PCI_Device device;
	PCI_GetPCIDevice(argv[1], &device);

	//Parse BAR0's key from the args		
	uint64_t bar5_key = 0;
	sscanf(argv[2], "B0:%*llx B1:%*llx B2:%*llx B3:%*llx B4:%*llx B5:%llx", &bar5_key);

	UserSharedMemoryData data;
	ApplySharedMemoryKey(bar5_key, &data);
	PCI_BAR = (uint64_t)data.VirtualAddress;

	//Not actually an AHCI device, exit
	if(PCI_BAR == 0)
		return -1;

	PCI_EnableBusMaster(&device);
	
	//Report that the OS is now AHCI aware
	ReportAHCIAwareness();
	ResetHBA();

	//Report AHCI awareness again in case the reset cleared the flag
	ReportAHCIAwareness();

	//Determine which ports are implemented by the HBA
	uint32_t ports_implemented = Read32(HBA_PI);

	//Allocate DMA memory assuming all ports are implemented
	uint64_t dma_buffer = 0;
	uint64_t dma_phys_addr = 0;
	MMap(&dma_buffer,
		 32 * (CMD_BUF_SIZE + FIS_SIZE),
		 MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
		 MMapFlags_DMA,
		 CachingModeWriteBack
		);
	R01_GetPhysicalAddress(0, dma_buffer, &dma_phys_addr);

	memset((void*)dma_buffer, 0, 32 * (CMD_BUF_SIZE + FIS_SIZE));

	//Initialize the active device bitmap
	activeDevices = 0;

	//Initialize the implemented ports
	for(int i = 0; i < 32; i++)
		if(ports_implemented & (1 << i))
			InitializePort(i, dma_phys_addr);

	while(1) {

		//Check for messages

	
		//Receive transmit request through writes
	
	
		//Respond to reads with receive buffers
	

	}

	return 0;
}