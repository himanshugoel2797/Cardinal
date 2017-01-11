/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>

#include "ihd.h"
#include "ihd_context.h"
#include "ironlake.h"
#include "display.h"
#include "backlight.h"

extern IHD_Context ctxt;

IHD_DisplayInfo *displays;
IHD_Pipe *pipes;
IHD_PanelFitter *panel_fitters;
IHD_FDITransmitterInfo *fdi_trans;

int
Display_Initialize(void) {
    displays = malloc(sizeof(IHD_DisplayInfo) * ctxt.max_displays);
    memset(displays, 0, sizeof(IHD_DisplayInfo) * ctxt.max_displays);

    pipes = malloc(sizeof(IHD_Pipe) * ctxt.max_pipes);
    memset(pipes, 0, sizeof(IHD_Pipe) * ctxt.max_pipes);

    panel_fitters = malloc(sizeof(IHD_PanelFitter) * ctxt.max_pfs);
    memset(panel_fitters, 0, sizeof(IHD_PanelFitter) * ctxt.max_pfs);

    fdi_trans = malloc(sizeof(IHD_FDITransmitterInfo) * ctxt.max_fdis);
    memset(fdi_trans, 0, sizeof(IHD_FDITransmitterInfo) * ctxt.max_fdis);

    //Enable all display hot plug detectors
    IHD_Write32(DISP_HOTPLUG_DETECT, (1 << DISP_HOTPLUG_DETECT_DPD_DETECT_ENABLE) | (1 << DISP_HOTPLUG_DETECT_DPC_DETECT_ENABLE) | (1 << DISP_HOTPLUG_DETECT_DPB_DETECT_ENABLE));

    //Unmask all interrupt status bits
    IHD_Write32(SOUTH_DISP_INTMSK, 0);

    for(int i = 0; i < ctxt.max_displays; i++) {

        displays[i].type = Display_GetDisplayType(i);

        if(Display_IsPresent(i)) {
            displays[i].isPresent = true;
            EDID_Read(i);

            if(displays[i].type == DisplayType_LVDS) {
                Backlight_SetActiveState(i, true);
                Backlight_SetPWMActiveState(i, true);
                displays[i].backlight.max_val = Backlight_GetMaxBacklightBrightness(i);
                Backlight_SetBacklightBrightness(i, displays[i].backlight.max_val / 2);
            }
        }
    }

    //Disable the vga display
    IHD_Write8(VGA_CLOCKING_MODE_CTRL, VGA_CLOCKING_MODE_SCREEN_OFF);

    //For now just set the display size to the pipe timings
    //The system will be responsible for selecting the final resolution
    //(reduces duplication of code like EDID parsing)


    //Disable the backlight
    Backlight_SetActiveState(LVDS_INDEX, false);

    //Power off the backlight pwm
    Backlight_SetPWMActiveState(LVDS_INDEX, false);

    //Disable panel power
    Display_SetPanelActiveState(LVDS_INDEX, false);

    //Disable the vga plane
    //Determine which pipe has been configured with the display and disable the vga plane
    uint32_t vga_plane_ctrl = IHD_Read32(VGA_PLANE_CTRL);
    vga_plane_ctrl |= (1 << VGA_PLANE_CTRL_DISABLE_BIT);
    IHD_Write32(VGA_PLANE_CTRL, vga_plane_ctrl);

    uint32_t vga_pipe_index = (vga_plane_ctrl >> VGA_PLANE_PIPE_SELECT_BIT) & 1;

    for(int i = 0; i < ctxt.max_pipes; i++){

		//Save all pipe timings    	
    	Display_SavePipeTimings(i, i);

    	//Disable all planes on each pipe
    	Display_SetDisplayPlaneActiveState(i, false);
    	CursorPlane_SetMode(i, CURSOR_PLANE_DISABLED);
    	Display_SetVideoPlaneActiveState(i, false);

    	//Disable all pipes
    	Display_SetPipeActiveState(i, false);
    }

    //Disable all panel fitters
    for(int i = 0; i < ctxt.max_pfs; i++)
    	Display_SetPanelFitterActiveState(i, false);

	//Allocate a pipe for the built-in display
    int alloc_pipe_index = Display_AllocatePipe(LVDS_INDEX);

    //Configure and enable the pipe based on the timings of the VGA pipe
    Display_RestorePipeTimings(vga_pipe_index, alloc_pipe_index);
    Display_SetPipeSize(alloc_pipe_index, pipes[alloc_pipe_index].hactive + 1, pipes[alloc_pipe_index].vactive + 1);

    //Enable the hires plane
    Display_SetDisplayPlaneActiveState(alloc_pipe_index, true);

    //Enable panel power
    Display_SetPanelActiveState(LVDS_INDEX, true);
    //TODO Wait for steady state

    //Enable backlight
    Backlight_SetActiveState(LVDS_INDEX, true);
    
    //Configure PWM unit
    Backlight_SetPWMActiveState(LVDS_INDEX, true);

    //Set the display brightness to half the maximum
    Backlight_SetBacklightBrightness(LVDS_INDEX, displays[LVDS_INDEX].backlight.max_val / 2);


	//TODO from here, start setting up GTTs, and devise context switching setup
	//Setup command ring buffers for all graphics engines

	

}

void
Display_SetPanelActiveState(int display, bool enable) {

}

void
Display_SetDisplayPlaneActiveState(int pipe_index, bool enable) {

}

void
CursorPlane_SetMode(int pipe_index, CURSOR_PLANE_MODES mode){
	if(pipe_index >= ctxt.max_pipes)
		return;

	pipes[pipe_index].cursor.mode = mode;

	uint32_t cursor_ctrl = IHD_Read32(CURSOR_PLANE_CTRL(pipe_index));
	cursor_ctrl &= CURSOR_PLANE_CTRL_MODE_SELECT_CLEAR_MASK;
	cursor_ctrl |= CURSOR_PLANE_CTRL_MODE_SELECT_CONV(mode);

	IHD_Write32(CURSOR_PLANE_CTRL(pipe_index), cursor_ctrl);
}

void
Display_SetVideoPlaneActiveState(int pipe_index, bool enable) {

}

int
Display_GetAssignedFDITransmitter(int display) {

}

int
Display_IsPresent(int display) {

    int retVal = 0;
    uint32_t reg_val = 0;

    //Check the port status bits
    //If that fails, check for a hotplug event

    //Still need to determine how to handle cases where the display has been disconnected

    switch(display) {
    case HDMI_C_INDEX:
        reg_val = IHD_Read32(HDMIC_PORT_CTRL);
        if(!PORT_CTRL_PORT_IS_PRESENT(reg_val)) {

            reg_val = IHD_Read32(SOUTH_DISP_INTSTS);
            if(reg_val & (1 << SOUTH_DISP_INT_DPC_HOTPLUG))
                retVal = 1;

        } else
            retVal = 1;
        break;
    case HDMI_D_INDEX:
        reg_val = IHD_Read32(HDMID_PORT_CTRL);
        if(!PORT_CTRL_PORT_IS_PRESENT(reg_val)) {

            reg_val = IHD_Read32(SOUTH_DISP_INTSTS);
            if(reg_val & (1 << SOUTH_DISP_INT_DPD_HOTPLUG))
                retVal = 1;

        } else
            retVal = 1;
        break;
    case LVDS_INDEX:
        retVal = PORT_CTRL_PORT_IS_PRESENT(IHD_Read32(LVDS_PORT_CTRL));
        break;
    }

    return retVal;
}

DisplayType
Display_GetDisplayType(int display) {
    switch(display) {
    case HDMI_C_INDEX:
        return DisplayType_HDMI;
        break;
    case HDMI_D_INDEX:
        return DisplayType_HDMI;
        break;
    case LVDS_INDEX:
        return DisplayType_LVDS;
        break;
    default:
        return DisplayType_Unknown;
        break;
    }
}

IHD_DisplayInfo*
Display_GetDisplay(int display) {
    if(display >= ctxt.max_displays)
        return NULL;

    return &displays[display];
}

bool
Display_CheckDisplaySupportsPipe(int display, int pipe){
	return true;
}

int
Display_AllocatePipe(int display) {
	for(int i = 0; i < ctxt.max_pipes; i++){
		if(pipes[i].enabled == false && Display_CheckDisplaySupportsPipe(display, i))
			return i;
	}
}

void
Display_SetPanelFitterActiveState(int pf_index, bool state) {
	if(pf_index >= ctxt.max_pfs)
		return;

	panel_fitters[pf_index].enabled = state;

	uint32_t pf_ctrl = IHD_Read32(PF_CTRL_1(pf_index));
	pf_ctrl &= ~(1 << PF_CTRL_ENABLE_SCALER);
	pf_ctrl |= (!!state << PF_CTRL_ENABLE_SCALER);
	IHD_Write32(PF_CTRL_1(pf_index), pf_ctrl);

}

void
Display_SetPanelFitterWindowRectangle(int pf_index, int x, int y, int w, int h, int vscale, int hscale) {

	if(pf_index >= ctxt.max_pfs)
		return;

	panel_fitters[pf_index].x = x;
	panel_fitters[pf_index].y = y;
	panel_fitters[pf_index].w = w;
	panel_fitters[pf_index].h = h;
	panel_fitters[pf_index].vscale = vscale;
	panel_fitters[pf_index].hscale = hscale;

	uint32_t pf_win_sz = IHD_Read32(PF_WIN_SZ(pf_index));
	uint32_t pf_win_pos = IHD_Read32(PF_WIN_POS(pf_index));

	pf_win_sz &= ~(PF_WIN_SZ_MASK << PF_WIN_WIDTH_OFF);
	pf_win_sz |= (w & PF_WIN_SZ_MASK) << PF_WIN_WIDTH_OFF;

	pf_win_sz &= ~(PF_WIN_SZ_MASK << PF_WIN_HEIGHT_OFF);
	pf_win_sz |= (h & PF_WIN_SZ_MASK) << PF_WIN_HEIGHT_OFF;

	pf_win_pos &= ~(PF_WIN_POS_MASK << PF_WIN_X_OFF);
	pf_win_pos |= (x & PF_WIN_POS_MASK) << PF_WIN_X_OFF;

	pf_win_pos &= ~(PF_WIN_POS_MASK << PF_WIN_Y_OFF);
	pf_win_pos |= (y & PF_WIN_POS_MASK) << PF_WIN_Y_OFF;

	//TODO Implement HSCALE and VSCALE

	IHD_Write32(PF_WIN_SZ(pf_index), pf_win_sz);		
	IHD_Write32(PF_WIN_POS(pf_index), pf_win_pos);

}

void
Display_SetPipeActiveState(int pipe_index, 
						   bool state) 
{

	if(pipe_index >= ctxt.max_pipes)
		return;

	state = !!state;

	//Setup the input bit as desired.
	uint32_t conf = IHD_Read32(PIPE_CONF(pipe_index));

	conf &= ~(1 << PIPE_CONF_ENABLE);
	conf |= (state << PIPE_CONF_ENABLE);

	IHD_Write32(PIPE_CONF(pipe_index), conf);

	//Wait for the pipe to power down
	while( (IHD_Read32(PIPE_CONF(pipe_index)) & (1 << PIPE_CONF_STATE)) != state)
		;

	pipes[pipe_index].enabled = state;
}

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
					   uint32_t vsync_end) 
{
	if(pipe_index >= ctxt.max_pipes)
		return -1;
	if(!(htotal & ~PIPE_TOTAL_TOTAL_MASK))
		return -2;
	//TODO add checks for the remaining arguments

	htotal = (htotal) & PIPE_TOTAL_TOTAL_MASK;
	hactive = (hactive) & PIPE_TOTAL_ACTIVE_MASK;
	hblank_start = (hblank_start) & PIPE_BLANK_START_MASK;
	hblank_end = (hblank_end) & PIPE_BLANK_END_MASK;
	hsync_start = (hsync_start) & PIPE_SYNC_START_MASK;
	hsync_end = (hsync_end) & PIPE_SYNC_END_MASK;

	vtotal = (vtotal) & PIPE_TOTAL_TOTAL_MASK;
	vactive = (vactive) & PIPE_TOTAL_TOTAL_MASK;
	vblank_start = (vblank_start) & PIPE_BLANK_START_MASK;
	vblank_end = (vblank_end) & PIPE_BLANK_END_MASK;
	vsync_start = (vsync_start) & PIPE_SYNC_START_MASK;
	vsync_end = (vsync_end) & PIPE_SYNC_END_MASK;

	//Setup the input bit as desired.
	pipes[pipe_index].htotal = htotal;
	pipes[pipe_index].hactive = hactive;
	pipes[pipe_index].hblank_start = hblank_start;
	pipes[pipe_index].hblank_end = hblank_end;
	pipes[pipe_index].hsync_start = hsync_start;
	pipes[pipe_index].hsync_end = hsync_end;

	pipes[pipe_index].vtotal = vtotal;
	pipes[pipe_index].vactive = vactive;
	pipes[pipe_index].vblank_start = vblank_start;
	pipes[pipe_index].vblank_end = vblank_end;
	pipes[pipe_index].vsync_start = vsync_start;
	pipes[pipe_index].vsync_end = vsync_end;	


	IHD_Write32(HTOTAL(pipe_index), (htotal << PIPE_TOTAL_TOTAL_OFF) | (hactive << PIPE_TOTAL_ACTIVE_OFF));
	IHD_Write32(HBLANK(pipe_index), (hblank_end << PIPE_BLANK_END_OFF) | (hblank_start << PIPE_BLANK_START_OFF));
	IHD_Write32(HSYNC(pipe_index), (hsync_end << PIPE_SYNC_END_OFF) | (hsync_start << PIPE_SYNC_START_OFF));

	IHD_Write32(VTOTAL(pipe_index), (vtotal << PIPE_TOTAL_TOTAL_OFF) | (vactive << PIPE_TOTAL_ACTIVE_OFF));
	IHD_Write32(VBLANK(pipe_index), (vblank_end << PIPE_BLANK_END_OFF) | (vblank_start << PIPE_BLANK_START_OFF));
	IHD_Write32(VSYNC(pipe_index), (vsync_end << PIPE_SYNC_END_OFF) | (vsync_start << PIPE_SYNC_START_OFF));

	return 0;
}

int
Display_SavePipeTimings(int pipe_index_src, int pipe_index_dst) 
{
	if((pipe_index_src >= ctxt.max_pipes) | (pipe_index_dst >= ctxt.max_pipes))
		return -1;

	uint32_t htotal_reg = IHD_Read32(HTOTAL(pipe_index_src));
	uint32_t hblank_reg = IHD_Read32(HBLANK(pipe_index_src));
	uint32_t hsync_reg = IHD_Read32(HSYNC(pipe_index_src));
	uint32_t vtotal_reg = IHD_Read32(VTOTAL(pipe_index_src));
	uint32_t vblank_reg = IHD_Read32(VBLANK(pipe_index_src));
	uint32_t vsync_reg = IHD_Read32(VSYNC(pipe_index_src));

	uint32_t htotal = (htotal_reg >> PIPE_TOTAL_TOTAL_OFF) & PIPE_TOTAL_TOTAL_MASK;
	uint32_t hactive = (htotal_reg >> PIPE_TOTAL_ACTIVE_OFF) & PIPE_TOTAL_ACTIVE_MASK;
	uint32_t hblank_start = (hblank_reg >> PIPE_BLANK_START_OFF) & PIPE_BLANK_START_MASK; 
	uint32_t hblank_end = (hblank_reg >> PIPE_BLANK_END_OFF) & PIPE_BLANK_END_MASK;
	uint32_t hsync_start = (hsync_reg >> PIPE_SYNC_START_OFF) & PIPE_SYNC_START_MASK;
	uint32_t hsync_end = (hsync_reg >> PIPE_SYNC_END_OFF) & PIPE_SYNC_END_MASK;
	uint32_t vtotal = (vtotal_reg >> PIPE_TOTAL_TOTAL_OFF) & PIPE_TOTAL_TOTAL_MASK;
	uint32_t vactive = (vtotal_reg >> PIPE_TOTAL_ACTIVE_OFF) & PIPE_TOTAL_ACTIVE_MASK;
	uint32_t vblank_start = (vblank_reg >> PIPE_BLANK_START_OFF) & PIPE_BLANK_START_MASK; 
	uint32_t vblank_end = (vblank_reg >> PIPE_BLANK_END_OFF) & PIPE_BLANK_END_MASK;
	uint32_t vsync_start = (vsync_reg >> PIPE_SYNC_START_OFF) & PIPE_SYNC_START_MASK;
	uint32_t vsync_end = (vsync_reg >> PIPE_SYNC_END_OFF) & PIPE_SYNC_END_MASK;
		
	//Setup the input bit as desired.
	pipes[pipe_index_dst].htotal = htotal;
	pipes[pipe_index_dst].hactive = hactive;
	pipes[pipe_index_dst].hblank_start = hblank_start;
	pipes[pipe_index_dst].hblank_end = hblank_end;
	pipes[pipe_index_dst].hsync_start = hsync_start;
	pipes[pipe_index_dst].hsync_end = hsync_end;

	pipes[pipe_index_dst].vtotal = vtotal;
	pipes[pipe_index_dst].vactive = vactive;
	pipes[pipe_index_dst].vblank_start = vblank_start;
	pipes[pipe_index_dst].vblank_end = vblank_end;
	pipes[pipe_index_dst].vsync_start = vsync_start;
	pipes[pipe_index_dst].vsync_end = vsync_end;	

	return 0;
}

int
Display_RestorePipeTimings(int pipe_index_src, int pipe_index_dst)
{
	if((pipe_index_src >= ctxt.max_pipes) | (pipe_index_dst >= ctxt.max_pipes))
		return -1;

	return Display_SetPipeTimings(pipe_index_src,
								  pipes[pipe_index_dst].htotal,
								  pipes[pipe_index_dst].hactive,
								  pipes[pipe_index_dst].hblank_start,
								  pipes[pipe_index_dst].hblank_end,
								  pipes[pipe_index_dst].hsync_start,
								  pipes[pipe_index_dst].hsync_end,
								  pipes[pipe_index_dst].vtotal,
								  pipes[pipe_index_dst].vactive,
								  pipes[pipe_index_dst].vblank_start,
								  pipes[pipe_index_dst].vblank_end,
								  pipes[pipe_index_dst].vsync_start,
								  pipes[pipe_index_dst].vsync_end);
}

int
Display_SetPipeSize(int pipe_index, 
					uint32_t w, 
					uint32_t h)
{
	if(pipe_index >= ctxt.max_pipes)
		return -1;

	if(w & ~PIPE_SZ_WIDTH_MASK)
		return -2;

	if(h & ~PIPE_SZ_HEIGHT_MASK)
		return -3;

	w = w & PIPE_SZ_WIDTH_MASK;
	h = h & PIPE_SZ_HEIGHT_MASK;

	pipes[pipe_index].w = w;
	pipes[pipe_index].h = h;

	IHD_Write32(PIPE_SZ(pipe_index), (w << PIPE_SZ_WIDTH_OFF) | (h << PIPE_SZ_HEIGHT_OFF));

	return 0;
}