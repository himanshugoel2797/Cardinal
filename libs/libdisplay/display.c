#include <cardinal/io/server.h>
#include <cardinal/namespace/server.h>
#include "display.h"

static UID display_pid;

int
Display_Initialize(void) {

	uint64_t error = 0;
    uint32_t tmp_key = 0;
    RetrieveNamespace("display", &tmp_key);
    while(!IsNamespaceRetrieved(tmp_key, &display_pid, &error));

    return (error != 0);
}

int
Display_GetDisplayCount(void) {
	uint64_t fd = 0;
	uint8_t key[KEY_BYTES];

	//Read the root directory and count the number of files that match displayXXX

	IO_Open(":", FileSystemOpFlag_Read, 0, key, display_pid, &fd);
    IO_Read(fd, 0, read_key, buf_len, pid);
    IO_Close(fd, pid);
}

int
Display_GetInfo(int display_index,
				DisplayInfo *disp_info) {
	//Parse the tags in the info of the display file
}

int
Display_GetDisplayImage(int display_index,
						uint64_t key) {
	//Read the display
}

uint64_t
Display_TryLockDisplay(int display_index) {
	//Try to open the display for writing, thus getting exclusive access
}

int
Display_UnlockDisplay(uint64_t display_key) {
	//Unlock the lock owned by this process on the display
}

void
Display_Update(uint64_t display_key,
			   uint64_t buffer_key) {
	//Write to the display
}