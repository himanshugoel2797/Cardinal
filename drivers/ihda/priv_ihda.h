#ifndef _IHDA_PRIV_H_
#define _IHDA_PRIV_H_

#include "types.h"

typedef struct {
    uint32_t addr_lo;
    uint32_t addr_hi;
    uint16_t wp;
    uint16_t rp;
    uint8_t ctrl;
    uint8_t sts;
    uint8_t size;
} __attribute__((packed))
RB_Bufs;

typedef struct {
    uint16_t gcap;
    uint8_t vmin;
    uint8_t vmaj;
    uint16_t outpay;
    uint16_t inpay;
    uint32_t gctl;
    uint16_t wakeen;
    uint16_t statests;
    uint16_t gsts;
    uint32_t unkn3;
    uint16_t unkn4;
    uint16_t outstrmpay;
    uint16_t instrmpay;
    uint32_t unkn0;
    uint32_t intctl;
    uint32_t intsts;
    uint32_t unkn2;
    uint32_t unkn5;
    uint32_t wallclock_ctr;
    uint32_t unkn1;
    uint32_t ssync;
    uint32_t unkn6;
    RB_Bufs corb;
    uint8_t unkn7;
    RB_Bufs rirb;
} __attribute__((packed))
IHDA_MMIO;

typedef struct {
    uint32_t nid;
    uint32_t type;
    uint32_t conn_list_len;
    uint32_t config_defaults;
    uint32_t first_conn_list_entry;
    uint32_t caps;
} IHDA_Widget;

typedef struct {
    uint32_t nid;
    uint32_t type;
    uint32_t sub_node_count;
    uint32_t start_node;
    IHDA_Widget *widgets;
} IHDA_AFG_Info;

typedef struct {
    IHDA_AFG_Info *afgs;
    uint32_t afg_count;
} IHDA_Codec;

#define VERB_GET (0xF00 << 8)
#define GET_SUB_NODE_COUNT 0x4
#define GET_FUNC_GROUP_TYPE 0x5
#define GET_AUDIO_WIDG_CAPS 0x9
#define GET_SUPP_PCM_SIZE_RATES 0xA
#define GET_SUPP_STRM_FORMATS 0xB
#define GET_PIN_CAP 0x0C
#define GET_IN_AMP_CAP 0x0D
#define GET_OUT_AMP_CAP 0x12
#define GET_CONN_LIST_LEN 0x0E
#define GET_VOL_KNOB_CAP 0x13

#define VERB_GET_ACTIV_CON 0xF01
#define VERB_SET_ACTIV_CON 0x701

#define VERB_GET_CONN_LIST_ENT (0xF02 << 8)
#define VERB_GET_AMP_GAIN 0x0B
#define VERB_SET_AMP_GAIN 0x03

#define VERB_GET_PIN_PROP 0xF07
#define VERB_SET_PIN_PROP 0x707

#define VERB_EAPD_ENABLE 0x70C
#define VERB_GET_VOL 0xF0F
#define VERB_SET_VOL 0x70F

#define VERB_GET_CONFIG_DEFAULT (0xF1C << 8)

uint32_t
IHDA_Get(uint8_t nid,
         uint8_t CAd,
         uint8_t param);

#endif