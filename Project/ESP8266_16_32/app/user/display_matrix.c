#include <display_matrix.h>
#include "fontcode.h"
#include "myTime.h"

MT *pNetTime;

typedef struct mTime{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t year_h;
	uint8_t year_l;
	uint8_t month;
	uint8_t day;
}M_Time;

M_Time gTime;

static uint8_t Matrix_5_7_Num[10][8] = {
		0x00, 0x0f, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0f, /*0*/
		0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, /*1*/
		0x00, 0x0f, 0x01, 0x01, 0x0f, 0x08, 0x08, 0x0f, /*2*/
		0x00, 0x0f, 0x01, 0x01, 0x0f, 0x01, 0x01, 0x0f, /*3*/
		0x00, 0x09, 0x09, 0x09, 0x0f, 0x01, 0x01, 0x01, /*4*/
		0x00, 0x0f, 0x08, 0x08, 0x0f, 0x01, 0x01, 0x0f, /*5*/
		0x00, 0x0f, 0x08, 0x08, 0x0f, 0x09, 0x09 ,0x0f, /*6*/
		0x00, 0x0f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /*7*/
		0x00, 0x0f, 0x09, 0x09, 0x0f, 0x09, 0x09, 0x0f, /*8*/
		0x00, 0x0f, 0x09, 0x09, 0x0f, 0x01, 0x01, 0x0f, /*9*/
};
static uint8_t Matrix_3_5_Num[10][5] = {
		0x07, 0x05, 0x05, 0x05, 0x07, /*0*/
		0x02, 0x02, 0x02, 0x02, 0x02, /*1*/
		0x07, 0x01, 0x07, 0x04, 0x07, /*2*/
		0x07, 0x01, 0x07, 0x01, 0x07, /*3*/
		0x05, 0x05, 0x07, 0x01, 0x01, /*4*/
		0x07, 0x04, 0x07, 0x01, 0x07, /*5*/
		0x07, 0x04, 0x07, 0x05, 0x07, /*6*/
		0x07, 0x01, 0x01, 0x01, 0x01, /*7*/
		0x07, 0x05, 0x07, 0x05, 0x07, /*8*/
		0x07, 0x05, 0x07, 0x01, 0x07, /*9*/
};

void ICACHE_FLASH_ATTR display_init(void)
{
	max7219_init();

	max7219_clear();
	max7219_setBrightness(0);
	max7219_full();

	gTime.second = 0;
	gTime.minute = 0;
	gTime.hour = 0;
	pNetTime = GetTime();
}

void time_caculate(void)
{
	if(pNetTime->bFind == 1)
	{
		pNetTime->bFind = 0;
		gTime.year_h = pNetTime->year_h;
		gTime.year_l = pNetTime->year_l;
		gTime.month = pNetTime->month;
		gTime.day = pNetTime->day;
		gTime.second = pNetTime->second;
		gTime.minute = pNetTime->minute;
		gTime.hour = pNetTime->hour;
	}
	else
	{
		if(gTime.second >= 59)
		{
			gTime.second = 0;
			if(gTime.minute >= 59)
			{
				gTime.minute = 0;
				if(gTime.hour >= 24)
				{
					gTime.hour = 0;
				}
				else
				{
					gTime.hour++;
				}
			}
			else
			{
				gTime.minute++;
			}
		}
		else
		{
			gTime.second++;
		}
	}
}


uint8_t x = 0,y = 1;
uint8_t code[16][4] =
{
	0x00,0x00,0x20,0x80,
	0x7F,0xFC,0x20,0x80,
	0x00,0x00,0x47,0xF8,
	0x00,0x00,0x90,0x80,
	0x3E,0xF8,0xF0,0x80,
	0x22,0x88,0x24,0x90,
	0x22,0x88,0x44,0x90,
	0x32,0xC8,0x84,0x90,
	0x2A,0xA8,0xF7,0xF0,
	0x2A,0xA8,0x00,0x80,
	0x22,0x88,0x00,0x80,
	0x22,0x88,0x30,0x88,
	0x22,0x88,0xC0,0x88,
	0x2E,0xB8,0x00,0x78,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
};


void uint8_to_uin32_t(uint8_t iLine, uint32_t *val)
{
	*val = code[iLine][0]<<24;
	*val |= code[iLine][1]<<16;
	*val |= code[iLine][2]<<8;
	*val |= code[iLine][3]&0xFF;
}

void time_display(uint8_t iLine, uint32_t *val)
{
	*val = 0;

	*val = Matrix_5_7_Num[gTime.hour/10][iLine] << 27;
	*val |= (Matrix_5_7_Num[gTime.hour%10][iLine] << 22);
	*val |= (Matrix_5_7_Num[gTime.minute/10][iLine] << 14);
	*val |= (Matrix_5_7_Num[gTime.minute%10][iLine] << 9);
	if(iLine > 2)
	{
		*val |= Matrix_3_5_Num[gTime.second/10][iLine-3] << 5;
		*val |= Matrix_3_5_Num[gTime.second%10][iLine-3] << 1;
	}
	if(iLine == 2 || iLine == 3 || iLine == 5 || iLine == 6)
	{
		*val |= 0x180000;
	}
}

void date_caculte(uint8_t iLine, uint32_t *val)
{
	*val = 0;
	if(iLine > 1 && iLine < 7)
	{
		*val = Matrix_3_5_Num[gTime.year_h/10][iLine-2] << 28;
		*val |= (Matrix_3_5_Num[gTime.year_h%10][iLine-2] << 24);
		*val |= (Matrix_3_5_Num[gTime.year_l/10][iLine-2] << 20);
		*val |= (Matrix_3_5_Num[gTime.year_l%10][iLine-2] << 16);
		*val |= (Matrix_3_5_Num[gTime.month/10][iLine-2] << 12);
		*val |= (Matrix_3_5_Num[gTime.month%10][iLine-2] << 8);
		*val |= (Matrix_3_5_Num[gTime.day/10][iLine-2] << 4);
		*val |= (Matrix_3_5_Num[gTime.day%10][iLine-2] << 0);
	}
}

void ICACHE_FLASH_ATTR display_time_pro(void *arg)
{
	static uint8_t i = 4;
		static uint32_t val;
		static uint32_t val1;
		time_caculate();

		for(i = 0; i < 8; i++)
		{
			time_display(i, &val);
			date_caculte(i, &val1);
			set_max7219_data(i+1, val, val1);
		}

		os_printf("display time %d%d-%d-%d %d:%d:%d \r\n", gTime.year_h, gTime.year_l, gTime.month, gTime.day, gTime.hour, gTime.minute, gTime.second);

}
