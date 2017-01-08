#include <cardinal/cardinal_types.h>

#include "ihd.h"
#include "ihd_regs.h"

int
GMBUS_DisableWriteProtect(void){
	uint32_t val = IHD_Read32(GMBUS_1);
	val &= ~(1 << 31);
	IHD_Write32(GMBUS_1, val);
}

int
GMBUS_I2C_Read(GMBUS_DEVICE device, int offset, int len, uint8_t *buf){

	GMBUS_RATE rate = GMBUS_RATE_100KHZ;
	int hold_time = 0;

	uint32_t gmbus_0_val = (rate << 8) | (hold_time << 7) | device;
	uint32_t gmbus_1_val = (1 << 25) /*WAIT*/ | (1 << 26) /*INDEX*/ | (len & 511) << 16 | (offset & 255) << 8 | 1 /*READ*/ | (1 << 30) /*SW ready*/;

	GMBUS_DisableWriteProtect();

	IHD_Write32(GMBUS_0, gmbus_0_val);
	IHD_Write32(GMBUS_1, gmbus_1_val);

	//Start reading the bytes
	for(int i = 0; i < len; i += 4){

		while(!(IHD_Read32(GMBUS_2) & (1 << 11)));

		uint32_t read_bytes = IHD_Read32(GMBUS_3);

		buf[i] = read_bytes & 0xff;
		if(len > i + 1)
			buf[i + 1] = (read_bytes >> 8) & 0xff;

		if(len > i + 2)
			buf[i + 2] = (read_bytes >> 16) & 0xff;

		if(len > i + 3)
			buf[i + 3] = (read_bytes >> 24) & 0xff;

		//Start the next round
		uint32_t sw_ready_val = IHD_Read32(GMBUS_1);
		IHD_Write32(GMBUS_1, sw_ready_val | (1 << 30));
	}

	return len;
}