#include "ihda.h"
#include "priv_ihda.h"
#include "utils/common.h"

static uint32_t ihda_bar;
static IHDA_MMIO *mmio;
static uint8_t codec_addresses[16];
static uint8_t codec_count;
static uint32_t *corb_buf;
static uint32_t *rirb_buf;
static uint32_t corb_entry_count, rirb_entry_count;
static IHDA_Codec codecs[16];

uint32_t
IHDA_Initialize(void)
{
    int i = 0, largest_i = -1, largest_codec_cnt = 0;
    ihda_bar = (uint32_t)virtMemMan_FindEmptyAddress(KB(4), MEM_KERNEL);

    for(; i <= pci_deviceCount; i++)
        {
            if(i == pci_deviceCount)return -1;
            if(pci_devices[i].classCode == 0x04 && pci_devices[i].subClassCode == 0x03)
                {
                    //Found what is likely an IHDA controller
                    uint32_t bar = pci_devices[i].bars[0] & ~(0xF);
                    //Map the BAR if necessary

                    if(bar < MEMIO_TOP_BASE)
                        {
                            physMemMan_MarkUsed(bar, KB(4));

                            if(virtMemMan_Map(ihda_bar,
                                              bar,
                                              KB(4),
                                              MEM_TYPE_UC,
                                              MEM_WRITE | MEM_READ,
                                              MEM_KERNEL) < 0)
                                return -1;
                        }
                    else
                        {
                            ihda_bar = VIRTUALIZE_HIGHER_MEM_OFFSET(bar);
                        }

                    mmio = (IHDA_MMIO*)ihda_bar;
                    IHDA_Reset();
                    for(int n = 0; n < 10000; n++);
                    COM_WriteStr("Bitcnt: %d, %b\r\n", set_bit_cnt(mmio->statests), mmio->statests);
                    if(set_bit_cnt(mmio->statests) >= largest_codec_cnt)
                        {
                            largest_i = i;
                            largest_codec_cnt = set_bit_cnt(mmio->statests);
                        }
                }
            if(largest_i != -1 && i == (pci_deviceCount - 1) )break;
        }

    pci_setCommand(largest_i, PCI_BUS_MASTER_CMD | 2);

    //Found what is likely an IHDA controller
    uint32_t bar = (pci_devices[largest_i].bars[0] & ~0xF);
    //Map the BAR if necessary

    if(bar < MEMIO_TOP_BASE)
        {
            physMemMan_MarkUsed(bar, KB(4));

            if(virtMemMan_Map(ihda_bar,
                              bar,
                              KB(4),
                              MEM_TYPE_UC,
                              MEM_WRITE | MEM_READ,
                              MEM_KERNEL) < 0)
                return -1;
        }
    else
        {
            ihda_bar = VIRTUALIZE_HIGHER_MEM_OFFSET(bar);
        }

    mmio = (IHDA_MMIO*)ihda_bar;
    IHDA_Reset();

    memset(codec_addresses, 0xFF, 16);
    IHDA_DetectCodecs();
    IHDA_SetupCORB();
    IHDA_SetupRIRB();

    i = 0;
    while(i < 16 && codec_addresses[i] != 0xFF)
        {
            //Get the AFG info
            uint32_t resp = IHDA_Get(0, i, GET_SUB_NODE_COUNT);
            uint32_t starting_node_num = (resp >> 16) & 0xFF;

            codecs[i].afg_count = resp & 0xFF;

            codecs[i].afgs =
                kmalloc(codecs[i].afg_count * sizeof(IHDA_AFG_Info));

            COM_WriteStr("AFG Count: %d\r\n", codecs[i].afg_count);
            COM_WriteStr("Vendor Info: %x\r\n", IHDA_Get(0, i, 1));

            for(int k = starting_node_num; k < starting_node_num + codecs[i].afg_count; k++)
                {

                    uint32_t i0 = k - starting_node_num;

                    uint32_t sub_node_dat = IHDA_Get(k, i, GET_SUB_NODE_COUNT);
                    uint32_t func_group_t = IHDA_Get(k, i, GET_FUNC_GROUP_TYPE);

                    codecs[i].afgs[i0].nid = k;
                    codecs[i].afgs[i0].type = func_group_t;
                    codecs[i].afgs[i0].sub_node_count = sub_node_dat & 0xFF;
                    codecs[i].afgs[i0].start_node = (sub_node_dat >> 16) & 0xFF;

                    codecs[i].afgs[i0].widgets = kmalloc(codecs[i].afgs[i0].sub_node_count * sizeof(IHDA_Widget));

                    COM_WriteStr("NID: %d, Sub Node Start: %d, Sub Node Count: %d\r\n",
                                 i0,
                                 codecs[i].afgs[i0].start_node,
                                 codecs[i].afgs[i0].sub_node_count);

                    for(int j = codecs[i].afgs[i0].start_node;
                            j < codecs[i].afgs[i0].start_node + codecs[i].afgs[i0].sub_node_count;
                            j++)
                        {

                            uint32_t i1 = j - codecs[i].afgs[i0].start_node;

                            func_group_t = IHDA_Get(j, i, GET_FUNC_GROUP_TYPE);
                            uint32_t a_widg_caps = IHDA_Get(j, i, GET_AUDIO_WIDG_CAPS);
                            uint32_t conn_ls_len = IHDA_Get(j, i, GET_CONN_LIST_LEN);


                            codecs[i].afgs[i0].widgets[i1].nid = j;
                            codecs[i].afgs[i0].widgets[i1].type = func_group_t;
                            codecs[i].afgs[i0].widgets[i1].conn_list_len = conn_ls_len;
                            codecs[i].afgs[i0].widgets[i1].caps = a_widg_caps;

                            IHDA_Verb verb;
                            verb.Verb = VERB_GET_CONN_LIST_ENT | 0;
                            verb.CAd = i;
                            verb.NID = j;
                            verb.IndirectNID = 0;

                            IHDA_WriteVerb(&verb);
                            codecs[i].afgs[i0].widgets[i1].first_conn_list_entry = IHDA_ReadResponse();

                            verb.Verb = VERB_GET_CONFIG_DEFAULT | 0;
                            IHDA_WriteVerb(&verb);
                            codecs[i].afgs[i0].widgets[i1].config_defaults = IHDA_ReadResponse();


                            switch((codecs[i].afgs[i0].widgets[i1].caps >> 20) & 0xF)
                                {
                                case 0x0:
                                    COM_WriteStr("Audio Output!\r\n");
                                    break;
                                case 0x1:
                                    COM_WriteStr("Audio Input!\r\n");
                                    break;
                                case 0x4:
                                    COM_WriteStr("Pin Complex!\r\n");
                                    break;
                                default:
                                    COM_WriteStr("Unknown Device: %x\r\n",
                                                 (codecs[i].afgs[i0].widgets[i1].caps >> 20) & 0xF);
                                    break;
                                }

                        }
                }

            i++;
        }

    return 0;
}

void
IHDA_Reset(void)
{
    mmio->statests = (1 << 16) - 1;
    mmio->gctl = 0;

    for(int n = 0; n < 6000; n++);

    mmio->gctl = 1;	//Write 1 to the CRST bit, to start the IHDA controller

    while(!(mmio->gctl & 1));	//Wait until the controller is out of reset

    int ticks = 0;
    while(mmio->statests == 0)
        {
            ticks++;
            if(ticks > 10000)break;
        }		//Wait a little more so the codecs can assert status change signals for codec detection
}

void
IHDA_DetectCodecs(void)
{
    uint16_t codecs = (uint16_t)mmio->statests;
    codec_count = 0;
    for(int i = 0; i < 16; i++)
        {
            if((codecs >> i) & 1)codec_addresses[codec_count++] = i;
        }
    COM_WriteStr("Detected Codecs: %b\r\n", codecs);
}

void
IHDA_SetupCORB(void)
{
    uint32_t corbsize = mmio->corb.size;	//Read the CORBSIZE register

    //Determine the largest supported size
    uint32_t supported_sizes = corbsize >> 4;
    uint32_t entry_count = 256;
    uint32_t entry_size = 4;	//One entry is 4 bytes

    if((supported_sizes >> 2) & 1)entry_count = 256;
    else if((supported_sizes >> 1) & 1)entry_count = 16;
    else entry_count = 2;

    //Stop the CORB DMA engine
    while( mmio->corb.ctrl & 2)mmio->corb.ctrl = 0;

    //Set the size of the CORB buffer
    corbsize &= ~3;

    if(entry_count == 16) corbsize |= 1;
    else if(entry_count == 256) corbsize |= 2;

    corbsize |= (entry_count / 16) & 3;	//Take the first 2 bits of the result of dividing by 16 to get the code
    mmio->corb.size = corbsize;

    //Allocate the needed size in DMA buffers and make sure it's KB aligned
    uint32_t base_addr = bootstrap_malloc(entry_count * entry_size + 128);
    base_addr += 128;
    base_addr -= (base_addr % 128);
    memset((void*)base_addr, 1, entry_size * entry_count);

    corb_buf = (uint32_t*)base_addr;
    corb_entry_count = entry_count;

    //Set the base address of the buffer
    mmio->corb.addr_lo = base_addr;
    mmio->corb.addr_hi = 0;

    //Reset the write pointer
    mmio->corb.wp = 0;

    //Reset the read pointer
    while((mmio->corb.rp >> 15) & 1 == 0)mmio->corb.rp |= (1 << 15);	//Wait till the controller has completed
    mmio->corb.rp = 0;					//Reset the bit
    while((mmio->corb.rp >> 15) & 1);		//Read back and make sure the bit has been set to 0

    while( (mmio->corb.ctrl & 2) != 2)mmio->corb.ctrl |= 2;	//Start the CORB DMA engine
}

void
IHDA_SetupRIRB(void)
{
    uint32_t corbsize = mmio->rirb.size;	//Read the CORBSIZE register

    //Determine the largest supported size
    uint32_t supported_sizes = corbsize >> 4;
    uint32_t entry_count = 256;
    uint32_t entry_size = 8;	//One entry is 4 bytes

    if((supported_sizes >> 2) & 1)entry_count = 256;
    else if((supported_sizes >> 1) & 1)entry_count = 16;
    else entry_count = 2;

    //Stop the CORB DMA engine
    while( mmio->rirb.ctrl & 2)mmio->rirb.ctrl = 0;

    //Set the size of the CORB buffer
    corbsize &= ~3;

    if(entry_count == 16) corbsize |= 1;
    else if(entry_count == 256) corbsize |= 2;

    mmio->rirb.size = corbsize;

    //Allocate the needed size in DMA buffers and make sure it's KB aligned
    uint32_t base_addr = bootstrap_malloc(entry_count * entry_size + 128);
    base_addr += 128;
    base_addr -= (base_addr % 128);
    memset((void*)base_addr, 0x22, entry_size * entry_count);

    rirb_buf = (uint32_t*)base_addr;
    rirb_entry_count = entry_count;

    //Set the base address of the buffer
    mmio->rirb.addr_lo = base_addr;
    mmio->rirb.addr_hi = 0;

    //set the read pointer
    mmio->rirb.rp = 1;

    //Reset the write pointer
    while((mmio->rirb.wp >> 15) & 1 == 0)mmio->rirb.wp |= (1 << 15);	//Wait till the controller has completed
    mmio->rirb.wp = 0;					//Reset the bit
    while((mmio->rirb.wp >> 15) & 1);		//Read back and make sure the bit has been set to 0

    while( (mmio->rirb.ctrl & 2) != 2)mmio->rirb.ctrl |= 2;	//Start the CORB DMA engine
}

void
IHDA_WriteVerb(IHDA_Verb* verb)
{
    while(1)
        {
            while( (mmio->corb.ctrl & 2) != 2)
                mmio->corb.ctrl |= 2;	//Start the CORB DMA engine

            uint32_t corb_write_pos = mmio->corb.wp & 0xFF;
            uint32_t corb_read_pos = mmio->corb.rp & 0xFF;

            if(corb_write_pos == corb_read_pos)	//Make sure that all commands so far have been sent
                {
                    uint32_t write_pos = (corb_write_pos + 1) % corb_entry_count;

                    corb_buf[write_pos] = *(uint32_t*)verb;
                    mmio->corb.wp = write_pos;
                    return;
                }
        }
}

uint32_t
IHDA_ReadResponse(void)
{
    while(1)
        {
            while((mmio->rirb.ctrl & 2) != 2)
                mmio->rirb.ctrl |= 2;	//Start the CORB DMA engine

            uint32_t rirb_write_pos = mmio->rirb.wp & 0xFF;
            uint32_t rirb_read_pos = mmio->rirb.rp & 0xFF;

            //if(rirb_write_pos != rirb_read_pos)
            {
                uint32_t read_pos = (rirb_read_pos + 1) % rirb_entry_count;

                uint32_t resp = rirb_buf[rirb_read_pos * 2];

                while((mmio->rirb.ctrl & 2) == 2)
                    mmio->rirb.ctrl &= ~2;	//Start the CORB DMA engine

                mmio->rirb.rp = read_pos;

                while((mmio->rirb.ctrl & 2) != 2)
                    mmio->rirb.ctrl |= 2;	//Start the CORB DMA engine

                return resp;
            }
        }
}

uint32_t
IHDA_Get(uint8_t nid,
         uint8_t CAd,
         uint8_t param)
{
    IHDA_Verb verb;
    verb.Verb = VERB_GET | param;
    verb.NID = nid;
    verb.IndirectNID = 0;
    verb.CAd = CAd;

    IHDA_WriteVerb(&verb);
    return IHDA_ReadResponse();
}