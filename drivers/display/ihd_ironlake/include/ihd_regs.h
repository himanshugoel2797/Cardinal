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

typedef enum {
    PP_CTRL_BACKLIGHT_PWR = 2
} PP_CTRL_BITS;

//Backlight control registers
#define PWM_PCH_CTRL 0xC8250
#define PWM_MOD_FREQ 0xC8254

typedef enum {
    PWM_PCH_CTRL_ENABLE = 31,
    PWM_PCH_CTRL_POLARITY = 29,
} PWM_PCH_CTRL_BITS;

#define BLC_PWM_CTL2 0x48250
#define BLC_PWM_CTL1 0x48254

typedef enum {
    BLC_PWM_CTL2_ENABLE = 31,
    BLC_PWM_CTL2_PIPE_ASSIGNMENT = 29,
} BLC_PWM_CTL2_BITS;

#define DISP_HOTPLUG_DETECT 0xC4030

#define SOUTH_DISP_INTMSK 0xC4004
#define SOUTH_DISP_INTSTS 0xC4008
#define SOUTH_DISP_INT_ENABLE 0xC400C

typedef enum {
    SOUTH_DISP_INT_DPD_HOTPLUG = 10,
    SOUTH_DISP_INT_DPC_HOTPLUG = 9,
    SOUTH_DISP_INT_DPB_HOTPLUG = 8,
} SOUTH_DISP_INT_BITS;

typedef enum {
    DISP_HOTPLUG_DETECT_DPD_DETECT_ENABLE = 20,
    DISP_HOTPLUG_DETECT_DPC_DETECT_ENABLE = 12,
    DISP_HOTPLUG_DETECT_DPB_DETECT_ENABLE = 4,
} DISP_HOTPLUG_DETECT_BITS;

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

//Panel fitter registers
#define PF_CTRL_1(i) (0x68080 + (0x800 * i))
#define PF_WIN_SZ(i) (0x68074 + (0x800 * i))
#define PF_WIN_POS(i) (0x68070 + (0x800 * i))

typedef enum {
    PF_CTRL_ENABLE_SCALER = 31,
} PF_CTRL_BITS;

typedef enum {
    PF_WIN_WIDTH_OFF = 16,
    PF_WIN_HEIGHT_OFF = 0,
} PF_WIN_SZ_OFFSETS;

typedef enum {
    PF_WIN_X_OFF = 16,
    PF_WIN_Y_OFF = 0,
} PF_WIN_POS_OFFSETS;

#define PF_WIN_SZ_MASK 0xFFF
#define PF_WIN_POS_MASK 0xFFF

//Pipe registers
#define HTOTAL(x) (0x60000)
#define HBLANK(x) (0x60004)
#define HSYNC(x) (0x60008)

#define VTOTAL(x) (0x6000C)
#define VBLANK(x) (0x60010)
#define VSYNC(x) (0x60014)

#define PIPE_SZ(x) (0x6001C)
#define VSYNCSHIFT(x) (0x60028)

typedef enum {
    PIPE_TOTAL_ACTIVE_OFF = 0,
    PIPE_TOTAL_TOTAL_OFF = 16,
} PIPE_TOTAL_BITS;

typedef enum {
    PIPE_BLANK_START_OFF = 0,
    PIPE_BLANK_END_OFF = 16
} PIPE_BLANK_BITS;

typedef enum {
    PIPE_SYNC_START_OFF = 0,
    PIPE_SYNC_END_OFF = 16
} PIPE_SYNC_BITS;

typedef enum {
    PIPE_SZ_WIDTH_OFF = 16,
    PIPE_SZ_HEIGHT_OFF = 0,
} PIPE_SZ_BITS;

#define PIPE_TOTAL_TOTAL_MASK 0x1FFF
#define PIPE_TOTAL_ACTIVE_MASK 0xFFF

#define PIPE_BLANK_START_MASK 0x1FFF
#define PIPE_BLANK_END_MASK 0x1FFF

#define PIPE_SYNC_START_MASK 0x1FFF
#define PIPE_SYNC_END_MASK 0x1FFF

#define PIPE_SZ_WIDTH_MASK 0xFFF
#define PIPE_SZ_HEIGHT_MASK 0xFFF


#define PIPE_DATA_M1(x) (0x60030)

#define PIPE_CONF(x) (0x70008 + (x * 0x1000))

typedef enum {
    PIPE_CONF_ENABLE = 31,
    PIPE_CONF_STATE = 30,
} PIPE_CONF_BITS;

//Plane Control registers
//VGA Plane Control
#define VGA_PLANE_CTRL 0x41000

typedef enum {
    VGA_PLANE_CTRL_DISABLE_BIT = 31,
    VGA_PLANE_PIPE_SELECT_BIT = 29,
} VGA_PLANE_CTRL_BITS;

//VGA registers
#define VGA_CLOCKING_MODE_CTRL 0x3C5

#define VGA_CLOCKING_MODE_SCREEN_OFF (1 << 5)

#define CURSOR_PLANE_CTRL(x) (0x70080 + (x * 0x40))
#define CURSOR_PLANE_BASE(x) (0x70084 + (x * 0x40))
#define CURSOR_PLANE_POS(x) (0x70088 + (x * 0x40))


typedef enum {
    CURSOR_PLANE_CTRL_ENABLE_BIT = 27,
    CURSOR_PLANE_CTRL_GAMME_ENABLE_BIT = 26,
    CURSOR_PLANE_CTRL_MODE_SELECT_UPPER_BIT = 5,
    CURSOR_PLANE_CTRL_MODE_SELECT_LOWER_BITS = 0,
} CURSOR_PLANE_CTRL_BITS;

#define CURSOR_PLANE_CTRL_MODE_SELECT_LOWER_MASK 0x7
#define CURSOR_PLANE_CTRL_MODE_SELECT_CLEAR_MASK ~((1 << CURSOR_PLANE_CTRL_MODE_SELECT_UPPER_BIT) | CURSOR_PLANE_CTRL_MODE_SELECT_LOWER_MASK)
#define CURSOR_PLANE_CTRL_MODE_SELECT_CONV(x) (((x >> 3) << CURSOR_PLANE_CTRL_MODE_SELECT_UPPER_BIT) | ((x & CURSOR_PLANE_CTRL_MODE_SELECT_LOWER_MASK) >> CURSOR_PLANE_CTRL_MODE_SELECT_LOWER_BITS))

//NOTE: There are more modes, but these are the only ones we care about. Check the PRM for more info.
typedef enum {
    CURSOR_PLANE_DISABLED = 0,
    CURSOR_PLANE_128_128_32_RGB_888_ANDINV = 0x2,
    CURSOR_PLANE_256_256_32_RGB_888_ANDINV = 0x3,
    CURSOR_PLANE_64_64_32_RGB_888_ANDINV = 0x7,
    CURSOR_PLANE_128_128_32_ARGB_8888 = 0xA,
    CURSOR_PLANE_256_256_32_ARGB_8888 = 0xB,
    CURSOR_PLANE_64_64_32_ARGB_8888 = 0xF,
} CURSOR_PLANE_MODES;



#endif