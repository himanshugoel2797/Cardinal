#ifndef _LIB_DISPLAY_H_
#define _LIB_DISPLAY_H_

#include <cardinal/cardinal_types.h>

#define DISPLAY_INFO_STRING "Device Name: %s\nWidth: %d\nHeight: %d\nPitch: %d\nBit Depth: %d\nR Offset: %d R Length: %d\nG Offset: %d G Length %d\nB Offset: %d B Length: %d\nA Offset: %d A Length: %d\n"

typedef struct {
	int width;
	int height;
	int pitch;
	int bpp;
	
	int R_offset;
	int R_length;
	
	int G_offset;
	int G_length;
	
	int B_offset;
	int B_length;
	
	int A_offset;
	int A_length;
} DisplayInfo;

int
Display_GetDisplayCount(void);

int
Display_GetInfoStringLength(int display_index);

int
Display_GetInfoString(int display_index, 
					  uint64_t key);

int
Display_GetInfo(int display_index,
				DisplayInfo *disp_info);

int
Display_GetDisplayImage(int display_index,
						uint64_t key);

uint64_t
Display_LockDisplay(int display_index);

int
Display_UnlockDisplay(uint64_t display_key);

void
Display_Update(uint64_t display_key,
			   uint64_t buffer_key);

#endif