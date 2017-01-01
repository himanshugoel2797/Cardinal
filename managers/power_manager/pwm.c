/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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