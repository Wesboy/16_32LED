#include "display_matrix.h"
#include "fontcode.h"

static uint8_t iRow[] = {0x01,0x02, 0x03,0x04, 0x5, 0x6, 0x7, 0x8};

void ICACHE_FLASH_ATTR display_init(void)
{
	max7219_init();

	max7219_clear();
	max7219_setBrightness(0);
	max7219_full();
}


uint8_t x = 0,y = 1;

void ICACHE_FLASH_ATTR display_pro(void *arg)
{
	static uint8_t pcs = 4;
	static uint32_t val = 0x80000000;
	max7219_clear();
	if(x < 8)
	{
		for(y = 0; y < 8; y++)
		{
			//max7219_setpox(1, y, mNumber[x][y]);
			//max7219_setpox(2, y, mNumber[(x+1)%8][y]);
		}
		x++;
	}
	else
	{
		x = 0;
		if(pcs <= 1)
			pcs = 4;
		else
			pcs--;
	}

	max7219_16_32_scan(val);
	val>>=1;
	if(!val)
		val = 0x80000000;

	os_printf("display cs:%d type%x bri:%d\r\n", pcs, val,y);

}
