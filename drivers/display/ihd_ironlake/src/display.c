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
#include "ihd_regs.h"
#include "ironlake.h"
#include "display.h"
#include "backlight.h"

extern IHD_Context ctxt;

IHD_DisplayInfo *displays;
IHD_Pipe *pipes;
IHD_PanelFitter *panel_fitters;
IHD_FDITransmitterInfo *fdi_trans;

int
Display_Initialize(void){
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

		if(Display_IsPresent(i)){
			displays[i].isPresent = true;
			EDID_Read(i);

			if(displays[i].type == DisplayType_LVDS){
				Backlight_SetActiveState(i, true);
				Backlight_SetPWMActiveState(i, true);
				displays[i].backlight.max_val = Backlight_GetMaxBacklightBrightness(i);
				Backlight_SetBacklightBrightness(i, displays[i].backlight.max_val / 2);
			}
		}
	}
}

int
Display_GetAssignedPipe(int display){

}

int
Display_GetAssignedFDITransmitter(int display){

}

int
Display_IsPresent(int display){

	int retVal = 0;
	uint32_t reg_val = 0;

	//Check the port status bits
	//If that fails, check for a hotplug event

	//Still need to determine how to handle cases where the display has been disconnected

	switch(display) {
		case HDMI_C_INDEX:
			reg_val = IHD_Read32(HDMIC_PORT_CTRL);
			if(!PORT_CTRL_PORT_IS_PRESENT(reg_val)){

				reg_val = IHD_Read32(SOUTH_DISP_INTSTS);
				if(reg_val & (1 << SOUTH_DISP_INT_DPC_HOTPLUG))
					retVal = 1;

			}else
				retVal = 1;
		break;
		case HDMI_D_INDEX:
			reg_val = IHD_Read32(HDMID_PORT_CTRL);
			if(!PORT_CTRL_PORT_IS_PRESENT(reg_val)){

				reg_val = IHD_Read32(SOUTH_DISP_INTSTS);
				if(reg_val & (1 << SOUTH_DISP_INT_DPD_HOTPLUG))
					retVal = 1;

			}else
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
	switch(display){
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
Display_GetDisplay(int display){
	if(display >= ctxt.max_displays)
		return NULL;

	return &displays[display];
}

int
Display_CreateDisplay(int display) {

}

void
Display_SetPanelFitterActiveState(int pf_index, bool state) {

}