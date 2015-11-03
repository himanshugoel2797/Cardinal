#ifndef _PIC_H_
#define _PIC_H_

#include "types.h"

#define PIC1    0x20    /* IO base address for master PIC */
#define PIC2    0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA (PIC2+1)

uint32_t PIC_Initialize();
void PIC_SetOffset(int off1, int off2);
void PIC_MaskAll();
void PIC_MaskIRQ(uint8_t irq);
void PIC_UnMaskIRQ(uint8_t irq);
void PIC_SendEOI(uint8_t irq);

#endif /* end of include guard: _PIC_H_ */
