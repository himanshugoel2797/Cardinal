#include "hpet.h"
#include "priv_hpet.h"

#include "acpi_tables/acpi_tables.h"

#include "utils/common.h"

uint64_t *hpet_base_addr;
uint8_t timer_count = 0;

uint8_t HPET_Initialize()
{
    hpet_base_addr = NULL;
    hpet = ACPITables_FindTable(HPET_SIG, 0);  //Find the HPET table
    frequency = 0;

    if(hpet == NULL) return -1; //No HPET, system should default to PIT
    hpet_base_addr = (uint64_t*)hpet->address.address;
    //if(hpet_base_addr == 0) hpet_base_addr = 0xFED00000;

    COM_WriteStr("%x          \r\n", hpet_base_addr);

    //Get HPET capabilities
    capabilities = HPET_Read(HPET_CAP_REG);
    timer_count = ((capabilities & 0x0F00) >> 8) + 1;

    //Calculate exp(10, 15)
    uint64_t numerator = 10;
    for(int i = 0; i < 15; i++) numerator *= 10;

    //Calculate the HPET frequency
    frequency = numerator/(capabilities >> 32);

    return 0;
}

uint8_t HPET_GetTimerCount()
{
    return timer_count;
}

uint64_t HPET_GetFrequency()
{
    return frequency;
}

void HPET_SetEnable(int enable)
{
    uint64_t gc = HPET_Read(HPET_GC_REG);
    gc = SET_VAL_BIT(gc, 0, (enable & 1));  //Enable HPET with standard mapping
    gc = SET_VAL_BIT(gc, 1, 1);  //Enable legacy mapping to replace the PIT
    HPET_Write(HPET_GC_REG, gc);
}

void HPET_Write(uint32_t reg, uint64_t val)
{
    hpet_base_addr[reg/8] = val;
}

uint64_t HPET_Read(uint32_t reg)
{
    return hpet_base_addr[reg/8];
}

uint64_t HPET_GetGlobalCounter()
{
    if(hpet != NULL)
        {
            return HPET_Read(HPET_MCV_REG);
        }
    return 0;
}

void HPET_SetGlobalCounter(uint64_t val)
{
    if(hpet != NULL)
        {
            HPET_Write(HPET_MCV_REG, val);
        }
}

uint32_t HPET_GetSupportedIRQs(uint8_t index)
{
    return (HPET_Read(TIMER_CONFIG_OFFSET(index)) >> 32);
}

uint8_t HPET_IsPeriodicCapable(uint8_t index)
{
    return (HPET_Read(TIMER_CONFIG_OFFSET(index)) >> 4) & 1;
}

void HPET_SetTimerConfig(uint8_t index, uint8_t irq, uint8_t enable_int, uint8_t periodic, uint8_t timer_value_set, uint64_t tick_count)
{
    uint64_t ccr = HPET_Read(TIMER_CONFIG_OFFSET(index));

    //Set the IRQ to use
    ccr &= ~(0xF << 9);
    ccr |= ((irq & 0xF) << 9);

    ccr &= ~(1 << 1); //Set Edge triggered

    ccr &= ~(1 << 2);
    ccr |= ((enable_int & 1) << 2); //Set interrupts

    ccr &= ~(1 << 3);
    ccr |= ((periodic & 1) << 3); //Set timer mode

    ccr &= ~(1 << 6);
    ccr |= ((timer_value_set & 1) << 6);  //Set timer_value_set bit

    HPET_Write(TIMER_CONFIG_OFFSET(index), ccr);
    HPET_Write(TIMER_COMP_VAL_OFFSET(index), tick_count);
}
