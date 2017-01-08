#ifndef _IHD_IRONLAKE_REGS_H_
#define _IHD_IRONLAKE_REGS_H_

//Steps to enable display
//Enable PCH clock reference source and PCH SSC modulator
//Wait for warmup
//To enable PCH display:
//	enable PCH FDI receiver PLL
//	wait for warmup + DMI latency
//	switch from rawclk to PCDclk in the FDI receiver (A or B)
//read the display edid
//enable cpu panel fitter if needed
//configure cpu pipe timings
//enable the cpu pipe
//configure and enable cpu planes (hires and cursor)
//train FDI:
//	set preemphasis and voltage, change values if steps fail
//	enable cpu fdi transmitter and pch fdi receiver
//	set training pattern 1
//	wait for training to finish
//	check if receive has been ack'd by checking fdi receiver isr
//	set training pattern 2
//	wait for training to finish
//	check if receive has been ack'd by checking fdi receiver isr
//	set normal pixel output
//	wait for idle pattern time
//	link is now active
//configure and enable pch dpll
//wait for dpll warmup
//configure transcoder timings
//enable transcoder 
//enable ports
//enable panel power
//wait for steady state
//disable panel power override
//enable panel backlight
//	configure pwm unit

//Steps to disable display
//Disable backlight
//	power off pwm unit
//disable panel power
//disable cpu planes
//disable cpu panel fitter
//disable cpu pipe
//wait for cpu pipe off
//disable fdi transmitter and receiver
//disable port
//disable transcoder
//wait for transcoder off
//disable pch dpll
//switch fdi receiver to rawclk
//disable fdi receiver pll
//disable ssc modulator
//disable pch clock reference source


//display initialization sequence from bios handoff - temporary
//disable backlight
//	power off pwm unit
//disable panel power
//disable vga plane
//configure and enable hires and cursor planes
//enable panel power
//	wait for steady state
//enable panel backlight
//	configure pwm unit


//Panel power sequencing register
#define PP_STAT 0xC7200
#define PP_CTRL 0xC7204
#define PP_ON_DELAYS 0xC7208
#define PP_OFF_DELAYS 0xC720C

#define PWM_PCH_ENABLE 0xC8250
#define PWM_MOD_FREQ 0xC8254

#define BLC_PWM_CTL2 0x48250
#define BLC_PWM_CTL1 0x48254


//GMBUS registers - read EDID
#define GMBUS_0 0xC5100
#define GMBUS_1 0xC5104
#define GMBUS_2 0xC5108
#define GMBUS_3 0xC510C
#define GMBUS_4 0xC5110
#define GMBUS_5 0xC5120

typedef enum {
	GMBUS_RATE_100KHZ = 0,
	GMBUS_RATE_50KHZ = 1,
	GMBUS_RATE_400KHZ = 2,
} GMBUS_RATE;

typedef enum {
	GMBUS_DEVICE_ANALOG = 2,
	GMBUS_DEVICE_LVDS = 3,
	GMBUS_DEVICE_PORTC = 4,
	GMBUS_DEVICE_PORTB = 5,
	GMBUS_DEVICE_PORTD = 6
} GMBUS_DEVICE;

//Port control registers
#define CRT_PORT_CTRL 0xE1100 
#define HDMIC_PORT_CTRL 0xE1150
#define HDMID_PORT_CTRL 0xE1160
#define LVDS_PORT_CTRL 0xE1180

#define PORT_CTRL_PORT_IS_PRESENT(val) (val & (1 << 2))


#endif