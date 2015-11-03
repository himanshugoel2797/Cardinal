#ifndef _CMOS_H_
#define _CMOS_H_

#include "types.h"

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t dayOfMonth;
    uint8_t month;
    uint16_t year;
    uint8_t century;
} RTC_Time;

void CMOS_Initialize();
void CMOS_GetRTCTime(RTC_Time *rtc);

#endif /* end of include guard: _CMOS_H_ */
