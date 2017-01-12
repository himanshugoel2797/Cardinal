#ifndef _IHD_DISPLAY_H_
#define _IHD_DISPLAY_H_

#include <cardinal/cardinal_types.h>
#include <stdbool.h>

#include "gmbus.h"
#include "ihd_regs.h"

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
    int mode;
    int x;
    int y;
    bool enabled;
} IHD_CursorPlane;

typedef struct {
    int pitch;
    bool enabled;
} IHD_DisplayPlane;

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

    uint32_t htotal;
    uint32_t hactive;
    uint32_t hblank_start;
    uint32_t hblank_end;
    uint32_t hsync_start;
    uint32_t hsync_end;

    uint32_t vtotal;
    uint32_t vactive;
    uint32_t vblank_start;
    uint32_t vblank_end;
    uint32_t vsync_start;
    uint32_t vsync_end;

    uint32_t w;
    uint32_t h;

    uint8_t bpp;
    uint8_t gamma_palette_mode;
    bool enabled;

    IHD_CursorPlane cursor;
    IHD_DisplayPlane display;
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
Display_AllocatePipe(int display);

bool
Display_CheckDisplaySupportsPipe(int display, int pipe);

void
Display_SetPanelActiveState(int display, bool enable);

void
Display_SetDisplayPlaneActiveState(int pipe_index, bool enable);

void
Display_SetCursorPlaneActiveState(int pipe_index, bool enable);

void
Display_SetVideoPlaneActiveState(int pipe_index, bool enable);

int
Display_SavePipeTimings(int pipe_index_src, int pipe_index_dst);

int
Display_RestorePipeTimings(int pipe_index_src, int pipe_index_dst);

int
Display_SetPipeSize(int pipe_index, 
                    uint32_t w, 
                    uint32_t h);

int
Display_SetPipeTimings(int pipe_index, 
                       uint32_t htotal,
                       uint32_t hactive,
                       uint32_t hblank_start, 
                       uint32_t hblank_end, 
                       uint32_t hsync_start,
                       uint32_t hsync_end, 
                       uint32_t vtotal,
                       uint32_t vactive, 
                       uint32_t vblank_start,
                       uint32_t vblank_end, 
                       uint32_t vsync_start,
                       uint32_t vsync_end);


void
Display_SetPipeActiveState(int pipe_index, 
                           bool state);

void
Display_SetPanelFitterWindowRectangle(int pf_index, int x, int y, int w, int h, int vscale, int hscale);

void
Display_SetPanelFitterActiveState(int pf_index, bool state);

void
CursorPlane_SetMode(int pipe_index, CURSOR_PLANE_MODES mode);

#endif