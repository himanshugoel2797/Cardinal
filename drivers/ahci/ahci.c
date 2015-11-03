#include "ahci.h"
#include "priv_ahci.h"
#include "managers.h"
#include "kmalloc.h"

#define READ_BUFFER_DMA_R_CMD_EXT 0x25
#define WRITE_DMA_R_CMD_EXT 0x35

static uint32_t ahci_memory_base = 0;
static uint32_t ahci_controller_index = 0;
static uint8_t port_count, cmd_count;
static HBA_MEM *hba_mem;
static uint8_t disks[32];



uint8_t
AHCI_Initialize(void)
{
    memset(disks, 0xff, 32);

    //Search for AHCI compatible controllers in the PCI device list
    for (int i = 0; i < pci_deviceCount; i++)
        {
            if (pci_devices[i].classCode == PCI_MASS_STORAGE_DEVICE_CLASS &&
                    pci_devices[i].subClassCode == 0x06)
                {
                    if (pci_devices[i].bar_count < 6)
                        continue;


                    ahci_controller_index = i;
                    //Found an AHCI compatible device!
                    COM_WriteStr("Found an AHCI controller!\r\n");
                    break;
                }
        }


    //Check to see if the base address has already been mapped due to the top half mapping scheme
    if (pci_devices[ahci_controller_index].bars[5] < MEMIO_TOP_BASE)
        {
            //Map the ahci memory base address into kernel memory
            ahci_memory_base = (uint32_t)virtMemMan_FindEmptyAddress(KB(8), MEM_KERNEL);

            if (ahci_memory_base == NULL)
                return -1;

            physMemMan_MarkUsed(pci_devices[ahci_controller_index].bars[5], KB(8));

            if (virtMemMan_Map(ahci_memory_base,
                               pci_devices[ahci_controller_index].bars[5],
                               KB(8),
                               MEM_TYPE_UC,
                               MEM_WRITE | MEM_READ,
                               MEM_KERNEL) < 0)
                return -1;
        }
    else
        {
            ahci_memory_base =
                VIRTUALIZE_HIGHER_MEM_OFFSET(pci_devices[ahci_controller_index].bars[5]);
        }

    //Enable PCI busmastering for this device
    pci_setCommand(ahci_controller_index, PCI_BUS_MASTER_CMD);

    hba_mem = (HBA_MEM*)ahci_memory_base;

    port_count = (hba_mem->cap & 0x0F) + 1;
    cmd_count = (hba_mem->cap >> 8) & 0x0F + 1;

    AHCI_Reset();

    COM_WriteStr ("AHCI Version: %d%d.%d%d\r\n",
                  (hba_mem->vs >> 24) & 0xFF,
                  (hba_mem->vs >> 16) & 0xFF,
                  (hba_mem->vs >> 8) & 0xFF,
                  hba_mem->vs & 0xFF);

    COM_WriteStr ("Ports: %b\r\n", hba_mem->pi);

    //Find the index of the first port that is an ATA Disk
    uint32_t drives_found = FALSE;
    for (uint32_t i = 0; i < port_count; i++)
        {
            if (((hba_mem->pi >> i) & 1) == 1 &&
                    AHCI_CheckDeviceType(&hba_mem->ports[i]) == AHCI_DEV_SATA)
                {
                    COM_WriteStr("Using port #%d\r\n", i);

                    uint32_t ahci_base_addr = bootstrap_malloc(KB(512));
                    ahci_base_addr += 1024;
                    ahci_base_addr -= (ahci_base_addr % 1024);

                    AHCI_RebasePort(&hba_mem->ports[i], ahci_base_addr, i);

                    disks[drives_found] = i;
                    drives_found++;
                }
        }

    COM_WriteStr("%d drives found!\r\n", drives_found);
    return drives_found;
}

void
AHCI_Reset(void)
{
    //Obtain ownership of the controller if ownershp handoff is supported
    if (hba_mem->cap2 & 1)
        {
            hba_mem->bohc |= 2;
            while((hba_mem->bohc & 1) || !(hba_mem->bohc & 2))
                ThreadMan_Yield();
        }

    //Software Reset the controller
    hba_mem->ghc |= (1 << 31);  //Enable AHCI
    hba_mem->ghc |= 1;  //Reset

    while (hba_mem->ghc & 1)
        ThreadMan_Yield();

    hba_mem->ghc |= (1 << 31);  //Re-enable AHCI
    hba_mem->ghc &= ~(1 << 1);  //Disable interrupts

    for(uint32_t i = 0; i < port_count; i++)
        {
            if(hba_mem->pi >> i)
                {
                    HBA_PORT *port = &hba_mem->ports[i];
                    AHCI_StopCMD(port);
                    port->serr = ~0;
                    port->cmd |= 2;
                    port->cmd |= 4;
                    for(int n = 0; n < 1000; n++);
                    port->is = ~0;
                    port->ie = 0;
                    port->cmd &= ~((1 << 27) | (1 << 26));
                    port->sctl |= 1;
                    for(int n = 0; n < 1000; n++);
                    port->sctl &= ~1;
                    port->is = ~0;
                    port->ie = 0;
                    AHCI_StartCMD(port);
                    port->serr = ~0;
                }
        }
}

uint8_t
AHCI_CheckDeviceType(HBA_PORT *port)
{
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = (ssts & 0x0F);

    if (ipm != HBA_PORT_IPM_ACTIVE | det != HBA_PORT_DET_PRESENT)
        return HBA_NO_DEVICE;


    switch (port->sig)
        {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
        }

}

bool
AHCI_0_Read(uint64_t start,
            uint32_t count,
            uint16_t *buf)
{
    //Determine the first present port
    if(disks[0] == 0xFF)return FALSE;
    return AHCI_Read(&hba_mem->ports[disks[0]], start, count, buf);
}

bool
AHCI_Read(HBA_PORT *port,
          uint64_t start,
          uint32_t count,
          uint16_t *buf)
{
    return AHCI_SendIOCommand(port, start, count, buf, FALSE);
}



bool
AHCI_0_Write(uint64_t start,
             uint32_t count,
             uint16_t *buf)
{
    if(disks[0] == 0xFF)return FALSE;
    return AHCI_Write(&hba_mem->ports[disks[0]], start, count, buf);
}

bool
AHCI_Write(HBA_PORT *port,
           uint64_t start,
           uint32_t count,
           uint16_t *buf)
{
    if(count & 511 != 0)count -= 512;   //Make it so that count gets rounded down
    return AHCI_SendIOCommand(port, start, count, buf, TRUE);
}



bool
AHCI_SendIOCommand(HBA_PORT *port,
                   uint64_t start,
                   uint32_t count,
                   uint16_t *buf,
                   bool write)
{
    count = ((count - 1) >> 9) + 1;   //Convert value into sector count

    port->is = ~0;       // Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = AHCI_FindCMDSlot(port);

    if (slot == -1)
        return FALSE;

    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;//[slot];
    cmdheader = &cmdheader[slot];

    cmdheader->cfl = sizeof (FIS_REG_H2D)/sizeof(uint32_t); // Command FIS size
    cmdheader->w = write & 1;                                   // Read from device
    cmdheader->prdtl = (uint16_t)((count-1)>>3) + 1;    // PRDT entries count

    HBA_CMD_TBL *cmd_tbl = (HBA_CMD_TBL*)cmdheader->ctba;
    //memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL) + sizeof(HBA_PRDT_ENTRY) * (count - 1));

    FIS_REG_H2D *fis = (FIS_REG_H2D*)cmd_tbl->cfis;

    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->pmport = 0;
    fis->rsv0 = 0;
    fis->c = 1;
    fis->command = (write)?WRITE_DMA_R_CMD_EXT:READ_BUFFER_DMA_R_CMD_EXT;

    fis->lba0 = start;
    fis->lba1 = start >> 8;
    fis->lba2 = start >> 16;
    fis->lba3 = start >> 24;
    fis->lba4 = start >> 32;
    fis->lba5 = start >> 40;

    fis->device = 1 << 6;
    fis->countl = count;
    fis->counth = count >> 8;

    int i = 0;
    for (; i < cmdheader->prdtl - 1; i++)
        {
            cmd_tbl->prdt_entry[i].dba = (uint32_t)virtMemMan_GetPhysAddress(buf, NULL);
            cmd_tbl->prdt_entry[i].dbau = 0;
            cmd_tbl->prdt_entry[i].rsv0 = 0;
            cmd_tbl->prdt_entry[i].dbc = KB(4) - 1;
            cmd_tbl->prdt_entry[i].rsv1 = 0;
            cmd_tbl->prdt_entry[i].i = 0;

            buf = ((uint32_t)buf) + KB(4);
            count -= 8;

        }

    cmd_tbl->prdt_entry[i].dba = (uint32_t)virtMemMan_GetPhysAddress(buf, NULL);
    cmd_tbl->prdt_entry[i].dbau = 0;
    cmd_tbl->prdt_entry[i].rsv0 = 0;
    cmd_tbl->prdt_entry[i].dbc = (count * 512) - 1;
    cmd_tbl->prdt_entry[i].rsv1 = 0;
    cmd_tbl->prdt_entry[i].i = 0;

    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
        {
            spin++;
        }
    if (spin == 1000000)
        {
            COM_WriteStr ("Port is hung\r\n");
            return FALSE;
        }

    port->ci = 1<<slot; // Issue command

    // Wait for completion
    while (1)
        {
            // In some longer duration reads, it may be helpful to spin on the DPS bit
            // in the PxIS port field as well (1 << 5)
            if ((port->ci & (1<<slot)) == 0)
                break;

            if (port->is & (1<<30))   // Task file error
                {
                    COM_WriteStr ("Task File Error while waiting for completion\r\n");
                    return FALSE;
                }
        }

    //COM_WriteStr("TESt %x\r\n", cmdheader);
    // Check again
    if (port->is & (1<<30))
        {
            COM_WriteStr ("Task File Error while returning\r\n");
            return FALSE;
        }

    return TRUE;
}

// Find a free command list slot
int
AHCI_FindCMDSlot(HBA_PORT *port)
{
    // If not set in SACT and CI, the slot is free
    uint32_t slots = (port->sact | port->ci);
    for (int i=0; i<cmd_count; i++)
        {
            if ((slots&1) == 0)
                return i;
            slots >>= 1;
        }
    COM_WriteStr ("Cannot find free command list entry\r\n");
    return -1;
}

void
AHCI_RebasePort(HBA_PORT *port,
                uint32_t AHCI_BASE,
                int portno)
{
    AHCI_StopCMD (port); // Stop command engine

    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = AHCI_BASE + (portno<<10);
    port->clbu = 0;
    memset ((void*)(port->clb), 0, 1024);


    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = AHCI_BASE + (32<<10) + (portno<<8);
    port->fbu = 0;
    memset ((void*)(port->fb), 0, 256);

    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
    for (int i=0; i<cmd_count; i++)
        {
            cmdheader[i].prdtl = 8; // 8 prdt entries per command table
            // 256 bytes per command table, 64+16+48+16*8
            // Command table offset: 40K + 8K*portno + cmdheader_index*256
            cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
            cmdheader[i].ctbau = 0;
            memset ((void*)cmdheader[i].ctba, 0, 256);
        }

    AHCI_StartCMD (port);    // Start command engine
    uint32_t c = port->cmd; //Flush the cmd buffer
    c = c;
}

// Start command engine
void
AHCI_StartCMD(HBA_PORT *port)
{
    // Wait until CR (bit15) is cleared
    while (port->cmd & (1<<15));

    // Set FRE (bit4) and ST (bit0)
    port->cmd |= 1<<4;
    port->cmd |= 1;
}

// Stop command engine
void
AHCI_StopCMD(HBA_PORT *port)
{
    // Clear ST (bit0)
    port->cmd &= ~1;

    // Clear FRE (bit4);
    port->cmd &= ~(1<<4);

    // Wait until FR (bit14), CR (bit15) are cleared
    while(1)
        {
            if (port->cmd & (1<<14))
                continue;
            if (port->cmd & (1<<15))
                continue;
            break;
        }
}