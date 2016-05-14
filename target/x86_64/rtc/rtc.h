#ifndef _RTC_X86_64_H_
#define _RTC_X86_64_H_

#include "types.h"

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t dayOfMonth;
    uint8_t month;
    uint16_t year;
    uint8_t century;
} RTC_Time;

void
RTC_Initialize(void);

void
RTC_GetRTCTime(RTC_Time *rtc);

#endif
