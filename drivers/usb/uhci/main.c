#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <stdio.h>
#include <string.h>

#define UHCI_CMD 0x0
#define UHCI_STS 0x2
#define UHCI_FRNUM 0x6
#define UHCI_FLBASEADD 0x8

static uint64_t PCI_BAR;

void
Write8(uint32_t off, uint8_t val) {
	outb(PCI_BAR + off, val);
}

uint8_t
Read8(uint32_t off) {
	return inb(PCI_BAR + off);
}

void
Write16(uint32_t off, uint16_t val) {
	outw(PCI_BAR + off, val);
}

uint16_t
Read16(uint32_t off) {
	return inw(PCI_BAR + off);
}

void
Write32(uint32_t off, uint32_t val) {
	outl(PCI_BAR + off, val);
}

uint32_t
Read32(uint32_t off) {
	return inl(PCI_BAR + off);
}

uint32_t
IsHalted(void) {
	return Read16(UHCI_STS) & (1 << 5);
}

void
InitController(void) {
	//Perform a global reset
	uint16_t cmd = Read16(UHCI_CMD);
	cmd |= (1 << 2);
	Write16(UHCI_CMD, cmd);

	while(Read16(UHCI_CMD) & (1 << 2))
		;

	//Reset the host controller
	cmd = Read16(UHCI_CMD);
	cmd |= (1 << 1);
	Write16(UHCI_CMD, cmd);

	while(Read16(UHCI_CMD) & (1 << 1))
		;
}

void
SetFrameNumber(uint16_t number) {
	Write16(UHCI_FRNUM, number);
}

void
SetFrameListBaseAddress(uint64_t base_addr) {
	Write32(UHCI_FLBASEADD, (uint32_t)base_addr);
}

int main(int argc, char *argv[]) {

	R01_GetIOPrivileges();	

	PCI_Device device;
	PCI_GetPCIDevice(argv[1], &device);

	PCI_BAR = PCI_GetBAR(&device, 4);
	
	PCI_EnableBusMaster(&device);

	InitController();

	//Setup receive buffer
	//Allocate 64K bytes for buffer
	uint64_t dma_buffer_vaddr = 0;
	uint64_t dma_phys_addr = 0;
	MMap(&dma_buffer_vaddr,
		 64 * 1024,
		 MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
		 MMapFlags_DMA,
		 CachingModeWriteBack
		);

	uint8_t *dma_buffer = (uint8_t*)dma_buffer_vaddr;

	//Initialize the frame list entries
	memset(dma_buffer, 0xFF, 1024 * 4);

	//Set the frame list base address
	SetFrameNumber(0);
	SetFrameListBaseAddress(dma_phys_addr);




	while(1) {
		//Check for device insertion/removal and send messages to main usb server

		//Check for messages

		//Perform any requested transactions
	
	}

	return 0;
}