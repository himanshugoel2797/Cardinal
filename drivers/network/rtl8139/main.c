#include <cardinal/driver_utils.h>
#include <pci/pci.h>

static uint64_t PCI_BAR;
static uint32_t isPCI_BAR_IO;

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
//		PCI_MapMemorySpaceBAR(PCI_BAR, 256);	//Spec says register space is 256 bytes
	}

	//Power On
	
	//Reset

//	PCI_EnableBusMaster(&device);

	//Setup receive buffer

	//Setup transmit buffer

	//Enable receiver and transmit, poll


	int i = 0;
	while(argv[0][i])
		outb(0x3f8, argv[0][i++]);

//	__asm__("hlt" :: "a"(argc));
}