#ifndef _IHD_DISPLAY_H_
#define _IHD_DISPLAY_H_

#include <cardinal/cardinal_types.h>
#include <stdbool.h>

#include "gmbus.h"

typedef enum {
	DisplayType_Unknown,
	DisplayType_LVDS,
	DisplayType_Analog,
	DisplayType_HDMI,
	DisplayType_DisplayPort
} DisplayType;

typedef struct {
	uint32_t htotal;
	uint32_t hblank;
	uint32_t hsync;
	uint32_t vtotal;
	uint32_t vblank;
	uint32_t vsync;
	uint32_t vsyncshift;
} IHD_DisplayTimings;

typedef struct {
	uint32_t src;
	uint32_t data_m1;
	uint32_t data_n1;
	uint32_t data_m2;
	uint32_t data_n2;
	uint32_t link_m1;
	uint32_t link_n1;
	uint32_t link_m2;
	uint32_t link_n2;
} IHD_Pipe;

typedef struct {
	uint32_t src;
	uint32_t data_m1;
	uint32_t data_n1;
	uint32_t data_m2;
	uint32_t data_n2;
	uint32_t link_m1;
	uint32_t link_n1;
	uint32_t link_m2;
	uint32_t link_n2;
} IHD_FDITransmitterInfo;

typedef struct {
	uint32_t max_val;
	uint32_t cur_val;
	uint32_t pipe_assignment;
	bool backlight_enabled;
	bool pwm_enabled;
} IHD_Backlight;

typedef struct {
	IHD_DisplayTimings timings;
	uint32_t res_w;
	uint32_t res_h;
	uint32_t res_p;
	uint32_t res_off;
	DisplayType type;
	int pipe_index;
	int fdi_trans_index;
	int fdi_recv_index;
	int gmbus_index;
	bool isPresent;
	IHD_Backlight backlight;
	uint8_t edid[EDID_LEN];
} IHD_DisplayInfo;

typedef struct {
	bool enabled;
	uint32_t w;
	uint32_t h;
	uint32_t x;
	uint32_t y;
	uint32_t vscale;
	uint32_t hscale;
} IHD_PanelFitter;

int
Display_Initialize(void);

int
Display_GetAssignedPipe(int display);

int
Display_GetAssignedFDITransmitter(int display);

int
Display_IsPresent(int display);

DisplayType
Display_GetDisplayType(int display);

IHD_DisplayInfo*
Display_GetDisplay(int display);

void
Display_SaveDisplayTimings(int display, IHD_DisplayInfo *dInfo);

void
Display_RestoreDisplayTimings(int display, IHD_DisplayInfo *dInfo);

int
Display_CreateDisplay(int display);

#endif