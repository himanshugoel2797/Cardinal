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
Write16(uint32_t off, uint16_t val) {
	if(isPCI_BAR_IO) 
	{
		outw(PCI_BAR + off, val);
	}else
	{
		*(uint16_t*)(PCI_BAR + off) = val;
	}
}

uint16_t
Read16(uint32_t off) {
	if(isPCI_BAR_IO) 
	{
		return inw(PCI_BAR + off);
	}else
	{
		return *(uint16_t*)(PCI_BAR + off);
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

typedef struct {
	uint8_t dst_mac[6];
	uint8_t src_mac[6];
	uint16_t type;
} __attribute__((packed)) Ethernet_Frame;

typedef struct {
	Ethernet_Frame frame;
	uint16_t hw_type;
	uint16_t protocol_type;
	uint8_t hw_addr_len;
	uint8_t protocol_addr_len;
	uint16_t opcode;
	uint8_t src_mac[6];
	uint8_t src_ip[4];
	uint8_t dst_mac[6];
	uint8_t dst_ip[4];
} __attribute__((packed)) ARP_Packet;

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
	MMap(&trn_dma_buffer,
		 5 * 1024,
		 MemoryAllocationFlags_Write | MemoryAllocationFlags_Read,
		 MMapFlags_DMA,
		 CachingModeWriteBack
		);

	R01_GetPhysicalAddress(0, trn_dma_buffer, &trn_dma_phys_addr);
	Write32(0x20, (uint32_t)trn_dma_phys_addr);	


	ARP_Packet *packet = (ARP_Packet*)trn_dma_buffer;

	for(int i = 0; i < 6; i++)
		packet->frame.dst_mac[i] = 0xFF;
	
	for(int i = 0; i < 6; i++)
		packet->frame.src_mac[i] = mac[i];

	packet->frame.type = 0x0608;

	packet->hw_type = 0x0100;
	packet->protocol_type = 0x0008;
	packet->hw_addr_len = 6;
	packet->protocol_addr_len = 4;
	packet->opcode = 0x0100;

	for(int i = 0; i < 6; i++)
		packet->src_mac[i] = mac[i];

	for(int i = 0; i < 4; i++)
		packet->src_ip[i] = 0;

	for(int i = 0; i < 6; i++)
		packet->dst_mac[i] = 0x00;

	for(int i = 0; i < 4; i++)
		packet->dst_ip[i] = 0x00;

	Write32(0x10, sizeof(ARP_Packet));

	while(!(Read32(0x10) & (1 << 15)))
		;

	while(Read16(0x3A) == 0)
		;
	__asm__("hlt" :: "a"(Read16(0x3A))); 

	while(1) {

		//Check for messages

	
		//Receive transmit request through writes
	
	
		//Respond to reads with receive buffers
	

	}

	return 0;
}