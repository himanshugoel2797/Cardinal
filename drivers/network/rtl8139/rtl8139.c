#include "rtl8139.h"
#include "drivers.h"
#include "managers.h"
#include "utils/native.h"

static uint32_t rtl8139_index = 0;
static uint8_t *rtl8139_mem_bar = 0;
static uint32_t io_bar = 0;

static uint8_t mac_addr[6];

static void
RTL8139_Outw(uint32_t offset,
             uint16_t val) {
    outw(io_bar + offset, val);
}

static void
RTL8139_Outl(uint32_t offset,
             uint32_t val) {
    outl(io_bar + offset, val);
}

static void
RTL8139_Outb(uint32_t offset,
             uint8_t val) {
    outb(io_bar + offset, val);
}

static uint8_t
RTL8139_Inb(uint32_t offset) {
    return inb(io_bar + offset);
}

static uint16_t
RTL8139_Inw(uint32_t offset) {
    return inw(io_bar + offset);
}

static uint32_t
RTL8139_Inl(uint32_t offset) {
    return inl(io_bar + offset);
}

static uint32_t
handleInterrupt(Registers *reg) {
    RTL8139_Outw(0x3E, 5);
    return 1;
}

static uint32_t
detectInterrupt(Registers *reg) {
    COM_WriteStr("Interrupt Recieved!!!\r\n");
    //Check if the ISR register reads how we expect it
    if((RTL8139_Inb(0x3C) & 4)) {
        //This is the interrupt we were looking for
        Interrupts_RegisterHandler(reg->int_no,
                                   Interrupts_GetFreeSlot(reg->int_no),
                                   handleInterrupt);

        Interrupts_RaiseOnUnhandled(NULL);  //Unregister this handler

        //Acknowledge the interrupt and return
        RTL8139_Outw(0x3E, 4);
    }

    return 1;
}


bool
RTL8139_Detect(uint32_t i) {
    //Official IDs
    if(pci_devices[i].vendorID == 0x10EC) {
        switch(pci_devices[i].deviceID) {
        case 0x8139:
        case 0x8339:
            return TRUE;
        }
    }

    if(pci_devices[i].vendorID == 0x1904) {
        switch(pci_devices[i].deviceID) {
        case 0x8139:
            return TRUE;
        }
    }

    //Unofficial IDs
    if(pci_devices[i].vendorID == 0x14EA && pci_devices[i].deviceID == 0xAB06) {
        return TRUE;
    }

    if(pci_devices[i].vendorID == 0x1186 && pci_devices[i].deviceID == 0x1300) {
        return TRUE;
    }

    if(pci_devices[i].vendorID == 0x1065 && pci_devices[i].deviceID == 0x8139) {
        return TRUE;
    }

    if(pci_devices[i].vendorID == 0x10BD && pci_devices[i].deviceID == 0x8139) {
        return TRUE;
    }

    if(pci_devices[i].vendorID == 0x1113 && pci_devices[i].deviceID == 0x1211) {
        return TRUE;
    }

    if(pci_devices[i].vendorID == 0x1259 && pci_devices[i].deviceID == 0x2503) {
        return TRUE;
    }

    return FALSE;   //The device wasn't found
}

static int transmit_index = 0;
static uint8_t *transmit_buffer = 0;

void
RTL8139_Transmit(void *packet,
                 uint16_t len) {
    int stat_off = 0x10 + sizeof(uint32_t) * transmit_index;
    uint32_t stat = RTL8139_Inl(stat_off);

    while(!((stat >> 15) & 1) && !((stat >> 13) & 1) ) {
        if((stat >> 30) & 1) {
            //Something went wrong
            COM_WriteStr("Failure!\r\n");
        }
        stat_off = 0x10 + sizeof(uint32_t) * transmit_index;    //Allow for the chance that the interrupt handler changes the value
        stat = RTL8139_Inl(stat_off);
        COM_WriteStr("status: %x, %d\r\n", stat, transmit_index);
    }

    //Copy while being careful to be thread safe
    memcpy(transmit_buffer + (stat_off - 0x10)/sizeof(uint32_t) * KB(2), packet, len);

    stat = len & 0x0fff;
    RTL8139_Outl(stat_off, stat);

    transmit_index = (transmit_index + 1) % 4;
}

void
RTL8139_Recieve(void) {
    uint16_t capr = RTL8139_Inw(0x38);
    uint16_t cbr = RTL8139_Inw(0x3A);

    if(cbr != capr) {
        COM_WriteStr("Packet recieved! %x, CAPR: %x, CBR: %x\r\n", *(uint32_t*)(transmit_buffer + capr), capr, cbr);
        RTL8139_Outw(0x38, cbr);
    }
}

uint32_t
RTL8139_Initialize(uint32_t deviceIndex) {
    //This device is too old to have MSI support, so use automatic legacy interrupt detection for this

    //Enable bus master
    pci_setCommand(deviceIndex, PCI_BUS_MASTER_CMD | (1 << 10));
    rtl8139_index = deviceIndex;
    io_bar = pci_devices[rtl8139_index].bars[0] & ~3;	//Use the MMIO interface

    COM_WriteStr("RTL8139IO Base: %x, %x\r\n", pci_devices[rtl8139_index].bars[0], pci_readDWord(pci_devices[rtl8139_index].bus, pci_devices[rtl8139_index].device, pci_devices[rtl8139_index].function, 0x3C));

    RTL8139_Outb(0x52, 0);  //Power on the NIC
    RTL8139_Reset();

    //Read the mac address
    for(int i = 0; i < 6; i++) {
        mac_addr[i] = RTL8139_Inb(i);
    }

    //Allocate 22 KB for the Rx buffer + Tx buffer
    uint32_t dma_base = bootstrap_malloc(KB(22));
    dma_base += KB(4);
    dma_base -= dma_base % KB(4);   //Align the address to page boundaries because we don't know anything about the alignment requirements
    memset(dma_base, 0, KB(10));

    dma_base += KB(10);
    transmit_buffer = dma_base;
    //Set all the TX buffer base addresses
    for(int i = 0; i < 4; i++) {
        RTL8139_Outl(0x20 + (i * sizeof(uint32_t)), dma_base);
        dma_base += KB(2);
    }

    RTL8139_Outl(0x30, dma_base);   //Set the Rx buffer address
    //RTL8139_Outw(0x3C, 0x0005);       //Enable TX and RX interrupts
    RTL8139_Outb(0x44, 0xf | (1 << 7)); //Enable all recieve modes and set the wrap bit

    RTL8139_Outw(0x38, 0);              //Set the CAPR and CBR registers to 0
    RTL8139_Outw(0x3A, 0);

    //Interrupts_RaiseOnUnhandled(detectInterrupt);  //register this handler
    RTL8139_Outb(0x37, 0x0C);           //Enable the RX and TX engines

    COM_WriteStr("MAC: %x:%x:%x:%x:%x:%x \r\n",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    //Send a packet through the driver to raise an interrupt in order to detect the interrupt for this NIC

    //Interrupts aren't working for now, so lets setup a timer to poll for packets
    UID tmr = Timers_CreateNew(FREQ(200), TRUE, RTL8139_Recieve);
    Timers_StartTimer(tmr);

    RTL8139_Transmit(transmit_buffer, 256);
    return mac_addr;    //If init success, return pointer to the mac address
}

uint32_t
RTL8139_Reset(void) {
    RTL8139_Outb(0x37, 0x10);   //Tell the NIC to come out of reset
    while((RTL8139_Inb(0x37) & 0x10) != 0); //Wait for the NIC to come out of reset
}