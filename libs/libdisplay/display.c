#include <cardinal/io/fileserver.h>
#include "display.h"

int
Display_GetDisplayCount(void) {
	
}

int
Display_GetInfoStringLength(int display_index) {

}

int
Display_GetInfoString(int display_index, 
					  uint64_t key) {

}

int
Display_GetInfo(int display_index,
				DisplayInfo *disp_info) {

}

int
Display_GetDisplayImage(int display_index,
						uint64_t key) {

}

uint64_t
Display_LockDisplay(int display_index) {

}

int
Display_UnlockDisplay(uint64_t display_key) {

}

void
Display_Update(uint64_t display_key,
			   uint64_t buffer_key) {

}