#ifndef __DS_3231__H__
#define __DS_3231__H__


#include <time.h>

#define DS3231_ADDRESS 0X68
#define SECONDREG 0X00
#define MINUTEREG 0X01
#define HOURREG 0X02
#define WTREG 0X03 //WEEKDAY
#define DATEREG 0X04
#define MONTHREG 0X05
#define YEARREG 0X06
#define ALARM_MIN1SECREG 0X07
#define ALARM_MIN1MINREG 0X08
#define ALARM_MIN1HRREG 0X09
#define ALARM_MIN1DATEREG 0X0A
#define ALARM_MIN2MINREG 0X0B
#define ALARM_MIN2HRREG 0X0C
#define ALARM_MIN2DATEREG 0X0D
#define CONTROLREG 0X0E
#define STATUSREG 0X0F
#define AGEOFFSETREG 0X10
#define TEMPMSBREG 0X11
#define TEMPLSBREG 0X12
#define _24_HOUR_FORMAT 0
#define _12_HOUR_FORMAT 1
#define AM 0
#define PM 1


void rtc_init(void);
void rtc_set(tm *tt);
void rtc2mez(tm *getTime);
float rtc_temp();

#endif