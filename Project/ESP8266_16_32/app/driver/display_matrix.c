#include "display_matrix.h"
#include "fontcode.h"


typedef struct mTime{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
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
}

void time_caculate(void)
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
	if(iLine < 8)
	{
		*val = Matrix_5_7_Num[gTime.hour/10][iLine] << 27;
		*val |= 0xFF00000&(Matrix_5_7_Num[gTime.hour%10][iLine] << 22);
		*val |= 0xFF000&(Matrix_5_7_Num[gTime.minute/10][iLine] << 13);
		*val |= 0xF00&(Matrix_5_7_Num[gTime.minute%10][iLine] << 8);
		if(iLine > 2)
		{
			*val |= Matrix_3_5_Num[gTime.second/10][iLine-3] << 4;
			*val |= Matrix_3_5_Num[gTime.second%10][iLine-3]&0xF;
		}
		if(iLine == 2 || iLine == 3 || iLine == 5 || iLine == 6)
		{
			*val |= 0x80000;
		}
	}
	else
	{
		*val = 0;
	}
}

void ICACHE_FLASH_ATTR display_pro(void *arg)
{
	static uint8_t i = 4;
	static uint32_t val = 0x80000000;
	static uint32_t val_tmp = 0;
	static uint8_t offset = 0;
	max7219_clear();
	val = 0;

	for(i = 0; i < 16; i++)
	{
		uint8_to_uin32_t(i, &val);
		val_tmp = val << (32-offset);
		val >>= offset;
		val |= val_tmp;
		max7219_16_32_scan(i+1, val);
	}
	if(offset < 32)
		offset++;
	else
		offset = 0;

	os_printf("display type%x \r\n", val);

}


void ICACHE_FLASH_ATTR display_time_pro(void *arg)
{
	static uint8_t i = 4;
		static uint32_t val;
		max7219_clear();
		time_caculate();
		val = 0;

		for(i = 0; i < 16; i++)
		{
			time_display(i, &val);
			max7219_16_32_scan(i+1, val);
		}

		os_printf("display time type%x \r\n", val);

}
