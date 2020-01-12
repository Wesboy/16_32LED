/*
 * myTime.h
 *
 *  Created on: 2020Äê1ÔÂ11ÈÕ
 *      Author: Administrator
 */

#ifndef APP_INCLUDE_MYTIME_H_
#define APP_INCLUDE_MYTIME_H_



typedef struct myTime{
	unsigned char hour;
	unsigned char is24Hor12;
	unsigned char minute;
	unsigned char second;
	unsigned char year_h;
	unsigned char year_l;
	unsigned char month;
	unsigned char day;
	unsigned char bFind;
}MT;


MT *GetTime(void);


#endif /* APP_INCLUDE_MYTIME_H_ */
