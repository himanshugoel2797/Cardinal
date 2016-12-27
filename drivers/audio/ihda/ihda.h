#ifndef _IHDA_H_
#define _IHDA_H_

#define IHDA_VMIN_REG 0x02
#define IHDA_VMAJ_REG 0x03
#define IHDA_OUTPAY_REG 0x04

#define IHDA_GCTL_REG 0x08

#define IHDA_CORB_LO_REG 0x40
#define IHDA_CORB_HI_REG 0x44
#define IHDA_CORB_WRITE_REG 0x48
#define IHDA_CORB_READ_REG 0x4a
#define IHDA_CORB_CTRL_REG 0x4c
#define IHDA_CORB_STS_REG 0x4d
#define IHDA_CORB_SZ_REG 0x4e

#define IHDA_RIRB_LO_REG 0x50
#define IHDA_RIRB_HI_REG 0x54
#define IHDA_RIRB_WRITE_REG 0x58
#define IHDA_RIRB_INTCNT_REG 0x5a
#define IHDA_RIRB_CTRL_REG 0x5c
#define IHDA_RIRB_STS_REG 0x5d
#define IHDA_RIRB_SZ_REG 0x5e

#define IHDA_DMA_LO_REG 0x70
#define IHDA_DMA_HI_REG 0x74

#define IHDA_SDnCTRL_REG (n) (0x80 + n * 0x20)
#define IHDA_SDnSTS_REG (n) (0x83 + n * 0x20)

#define IHDA_8B_SUP_BIT (1 << 4)
#define IHDA_64B_SUP_BIT (1 << 5)
#define IHDA_1024B_SUP_BIT (1 << 6)

#define IHDA_8B_VAL 0x00
#define IHDA_64B_VAL 0x01
#define IHDA_1024B_VAL 0x02

#define IHDA_CORB_RUN (1 << 1)
#define IHDA_RIRB_RUN (1 << 1)

#define IHDA_GCTL_RESET 1

#define IHDA_MINOR_VER 0x00
#define IHDA_MAJOR_VER 0x01
#define IHDA_OUTPAY_RESETVAL 0x3C

#endif