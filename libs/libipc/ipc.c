#include "ipc.h"

static uint32_t msg_id = 1;

uint32_t
RequestMessageID(void) {
	return __sync_fetch_and_add(&msg_id, 1);
}