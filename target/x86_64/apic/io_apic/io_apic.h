#ifndef _IO_APIC_DRIVER_H_
#define _IO_APIC_DRIVER_H_

#include "types.h"

/**
 * \addtogroup ioapic_driver IO APIC driver
 * @{
*/

#define MAX_IOAPIC_COUNT 128
#define IOAPIC_PIN_COUNT 24

uint8_t
IOAPIC_Initialize(uint64_t baseAddr,
                  uint32_t global_int_base);

void
IOAPIC_MapIRQ(uint8_t global_irq,
              uint8_t apic_vector,
              uint64_t apic_id,
              uint8_t trigger_mode,
              uint8_t polarity,
              uint8_t delivery_mode);

void
IOAPIC_SetEnableMode(uint8_t vector,
                     bool active);

uint8_t
IOAPIC_GetVector(uint8_t global_irq,
                 uint8_t *vec);

void
IOAPIC_VirtualizeAll(void);

/**@}*/

#endif /* end of include guard: _IO_APIC_DRIVER_H_ */
