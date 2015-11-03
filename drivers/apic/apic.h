#ifndef _APIC_DRIVER_H_
#define _APIC_DRIVER_H_

#include "types.h"
#include "io_apic/io_apic.h"

/**
 * \defgroup apic_driver APIC driver
 * @{
*/

#define APIC_TIMER 0x320
#define APIC_THERMAL_SENSOR 0x330
#define APIC_PERF_MON 0x340
#define APIC_LINT0    0x350
#define APIC_LINT1    0x360
#define APIC_ERR      0x370
#define APIC_TIMER_VAL 0x380

#define APIC_TIMER_ONESHOT 0x0
#define APIC_TIMER_PERIODIC 0x1
#define APIC_TIMER_TSC      0x2

#define APIC_DELIVERY_MODE_FIXED 0x0
#define APIC_DELIVERY_MODE_SMI  0x2
#define APIC_DELIVERY_MODE_NMI  0x4
#define APIC_DELIVERY_MODE_EXTINT 0x7
#define APIC_DELIVERY_MODE_INIT  0x5

#define APIC_TRIGGER_MODE_EDGE 0
#define APIC_TRIGGER_MODE_LEVEL 1

#define APIC_POLARITY_HIGH 0
#define APIC_POLARITY_LOW  1

uint8_t
APIC_LocalInitialize(void);

uint32_t
APIC_Initialize(void);

void
APIC_SetEnableMode(uint8_t enabled);

uint8_t
APIC_GetID(void);

void
APIC_SetTimerMode(uint8_t mode);

void
APIC_SetDeliveryMode(uint32_t lvt_entry,
                     uint8_t mode);

void
APIC_SetTriggerMode(uint32_t lvt_entry,
                    uint8_t mode);

void
APIC_SetPolarity(uint32_t lvt_entry,
                 uint8_t mode);

void
APIC_SetTimerValue(uint32_t val);

uint32_t
APIC_GetTimerValue(void);

void
APIC_SetTimerDivisor(uint8_t divisor);

void
APIC_SetEnableInterrupt(uint32_t interrupt,
                        int enableMode);

void
APIC_SetVector(uint32_t interrupt,
               uint8_t vector);

void
APIC_SendEOI(uint8_t int_num);

void
APIC_Virtualize(void);

/**@}*/

#endif /* end of include guard: _APIC_DRIVER_H_ */
