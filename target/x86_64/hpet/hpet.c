#include "acpi/acpi_tables.h"
#include "acpi/hpet.h"
#include "priv_hpet.h"

static
HPET_Main* hpet_regs = NULL;

HPETError
HPET_Initialize(void)
{
	HPET *hpet = (HPET*)ACPITables_FindTable(HPET_SIG, 0);
	if(hpet == NULL)return HPETError_NotPresent;

	hpet_regs = (HPET_Main*)hpet->Address.address;
	HPET_SetCounterEnableStatus(DISABLE);
	HPET_SetCounterValue(0);
}

void
HPET_SetCounterEnableStatus(bool enableStatus)
{
	hpet_regs->Configuration.GlobalEnable = enableStatus;
}

uint64_t
HPET_GetElapsedTime(uint64_t diff)
{
	return (diff * HPET_GetPeriod())/1000000;
}

uint64_t
HPET_GetPeriod(void)
{
	return hpet_regs->Capabilities.ClockPeriod;
}

uint64_t
HPET_GetCounterValue(void)
{
	return hpet_regs->CounterValue;
}

void
HPET_SetCounterValue(uint64_t val)
{
	hpet_regs->CounterValue = val;
}