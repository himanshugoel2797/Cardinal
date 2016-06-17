#include "pwm.h"
#include "list.h"
#include "common.h"

static List *pwm_data;

void
PWM_Initialize(void) {
    pwm_data = List_Create(CreateSpinlock());
}


PWMError
PWM_RegisterStateChangeHandler(PWM_StateChangeHandler handler) {
    if(List_AddEntry(pwm_data, handler) == ListError_None)return PWMError_Success;
    else return PWMError_Error;
}

void
PWM_UnregisterHandler(PWM_StateChangeHandler handler) {
    for(uint64_t i = 0; i < List_Length(pwm_data); i++) {
        if(List_EntryAt(pwm_data, i) == handler) {
            List_Remove(pwm_data, i);
            i--;
        }
    }
}