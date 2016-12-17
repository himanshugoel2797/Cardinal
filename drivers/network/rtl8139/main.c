#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>

static uint64_t PCI_BAR;
static uint32_t isPCI_BAR_IO;

void
Write8(uint32_t off, uint8_t val) {
	if(isPCI_BAR_IO) 
	{
		outb(PCI_BAR + off, val);
	}else
	{
		*(uint8_t*)(PCI_BAR + off) = val;
	}
}

uint8_t
Read8(uint32_t off) {
	if(isPCI_BAR_IO) 
	{
		return inb(PCI_BAR + off);
	}else
	{
		return *(uint8_t*)(PCI_BAR + off);
	}
}

void
Write32(uint32_t off, uint32_t val) {
	if(isPCI_BAR_IO) 
	{
		outl(PCI_BAR + off, val);
	}else
	{
		*(uint32_t*)(PCI_BAR + off) = val;
	}
}

uint32_t
Read32(uint32_t off) {
	if(isPCI_BAR_IO) 
	{
		return inl(PCI_BAR + off);
	}else
	{
		return *(uint32_t*)(PCI_BAR + off);
	}
}


int main(int argc, char *argv[]) {

	R01_GetIOPrivileges();	

	PCI_Device device;
	PCI_GetPCIDevice(argv[1], &device);


	PCI_BAR = PCI_GetBAR(&device, 0);
	isPCI_BAR_IO = PCI_IsIOSpaceBAR(&device, 0);

	if(PCI_BAR == 0)
	{
		PCI_BAR = PCI_GetBAR(&device, 1);
		isPCI_BAR_IO = PCI_IsIOSpaceBAR(&device, 1);
	}

	if(!isPCI_BAR_IO)
	{
		//Parse BAR0's key from the args
		uint64_t bar0_key = 0;

		sscanf(argv[2], "B0:%x", &bar0_key);

		UserSharedMemoryData data;
		ApplySharedMemoryKey(bar0_key, &data);

		PCI_BAR = (uint64_t)data.VirtualAddress;
	}

	PCI_EnableBusMaster(&device);
	
	//Power On
	Write8(0x52, 0x0);

	//Reset
	Write8(0x37, 0x10);
	while(Read8(0x37) & 0x10);

	//Setup receive buffer
	//Allocate 32K + 16 + 1500 bytes for buffer
	/*uint64_t dma_buffer = 0;
	uint64_t dma_phys_addr = 0;
	MMap(&dma_buffer,
		 36 * 1024,
		 MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
		 MMapFlags_DMA,
		 CachingModeWriteBack
		);
	*/
	//Configure the receive buffer to receive all broadcast packets, but no promiscious mode
	//Buffer size = 32K + 16 bytes + 1500 bytes
	Write32(0x44, (2 << 11) | 0xE | (1 << 7));

	//Enable receiver and transmit, poll
	Write8(0x37, 0x0C);

	while(1) {

		//Check for messages

	
		//Receive transmit request through writes
	
	
		//Respond to reads with receive buffers
	

	}

	return 0;
}