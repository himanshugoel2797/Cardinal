#include "ihd.h"
#include "ihd_regs.h"

#include "ironlake.h"
#include "display.h"
#include "backlight.h"

extern IHD_Backlight backlight;

void
Backlight_SetPWMActiveState(int index, bool active) {

    IHD_DisplayInfo *disp = Display_GetDisplay(index);
    if(disp == NULL || disp->type != DisplayType_LVDS)
        return;

    active = !!active;

    uint32_t backlight_pwm_ctrl = IHD_Read32(BLC_PWM_CTL2);
    backlight_pwm_ctrl &= ~(1 << BLC_PWM_CTL2_ENABLE);
    backlight_pwm_ctrl |= (active << BLC_PWM_CTL2_ENABLE);

    //Store the pipe assignment
    disp->backlight.pipe_assignment = (backlight_pwm_ctrl >> BLC_PWM_CTL2_PIPE_ASSIGNMENT) & 1;
    disp->backlight.pwm_enabled = active;

    IHD_Write32(BLC_PWM_CTL2, backlight_pwm_ctrl);

}

int
Backlight_GetMaxBacklightBrightness(int index) {

    IHD_DisplayInfo *disp = Display_GetDisplay(index);
    if(disp == NULL || disp->type != DisplayType_LVDS)
        return 0;

    //Get the modulation frequency
    uint32_t pwm_mod_freq = IHD_Read32(PWM_MOD_FREQ);
    disp->backlight.max_val = pwm_mod_freq >> 16;

    return disp->backlight.max_val;
}

void
Backlight_SetBacklightBrightness(int index, int val) {

    IHD_DisplayInfo *disp = Display_GetDisplay(index);
    if(disp == NULL || disp->type != DisplayType_LVDS)
        return;

    if(val > disp->backlight.max_val)
        return;

    //Set the modulation frequency
    IHD_Write32(BLC_PWM_CTL1, val & 0xFFFF);
    disp->backlight.cur_val = val & 0xFFFF;
}

void
Backlight_SetActiveState(int index, bool active) {

    IHD_DisplayInfo *disp = Display_GetDisplay(index);
    if(disp == NULL || disp->type != DisplayType_LVDS)
        return;

    disp->backlight.backlight_enabled = active;

    active = !!active;

    uint32_t backlight_pwm_ctrl = IHD_Read32(PP_CTRL);
    backlight_pwm_ctrl &= ~(1 << PP_CTRL_BACKLIGHT_PWR);
    backlight_pwm_ctrl |= (active << PP_CTRL_BACKLIGHT_PWR);

    IHD_Write32(PP_CTRL, backlight_pwm_ctrl);
}

