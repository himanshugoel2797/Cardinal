#ifndef _PRIV_APIC_DRIVER_H_
#define _PRIV_APIC_DRIVER_H_

#include "types.h"

/**
 * \addtogroup apic_driver APIC driver
 * @{
*/

#define IA32_APIC_BASE 0x01B

#define APIC_ID 0x020
#define APIC_EOI 0x0B0
#define APIC_SVR 0x0F0
#define APIC_ISR_BASE 0x100
#define APIC_TPR 0x080
#define APIC_INITIAL_COUNT 0x380

void
APIC_FillHWInterruptHandler(char *idt_handler,
                            uint8_t intNum,
                            uint8_t irqNum);

void
APIC_DefaultHandler(void);

void
APIC_Write(uint32_t reg,
           uint32_t val);

uint32_t
APIC_Read(uint32_t reg);

/**@}*/

#endif /* end of include guard: _PRIV_APIC_DRIVER_H_ */
