
#include "driver/uart.h"  //串口0需要的头文件
#include "osapi.h"  //串口1需要的头文件
#include "user_interface.h" //WIFI连接需要的头文件
#include "gpio.h"  //端口控制需要的头文件
#include "max7219.h"
#include "fontcode.h"


os_timer_t timer;
uint8 x = 0,y = 1;

void ICACHE_FLASH_ATTR display(void *arg)
{
	static uint8_t pcs = 1;
	max7219_clear();
	if(x < 10)
	{
		for(y = 1; y < 9;y++)
		{
			max7219_write_cascade(1, y, Number[x][y-1]);
			max7219_write_cascade(2, y, Number[x][y-1]);
			max7219_write_cascade(3, y, Number[x][y-1]);
			max7219_write_cascade(4, y, Number[x][y-1]);
			max7219_write_cascade(5, y, Number[x][y-1]);
			max7219_write_cascade(6, y, Number[x][y-1]);
			max7219_write_cascade(7, y, Number[x][y-1]);
			max7219_write_cascade(8, y, Number[x][y-1]);
		}
		x++;
	}
	else
	{
		x = 0;
	}
	if(pcs >= 8)
		pcs = 1;
	else
		pcs++;

	//max7219_setval(pcs, type);
	//max7219_setBrightness(x);
	os_printf("display cs:%d type%d bri:%d\r\n", pcs, x,y);



}
void user_init()
{
	max7219_init();

	max7219_clear();
	max7219_setBrightness(0);
	os_timer_disarm(&timer);
	os_timer_setfn(&timer,display,NULL);
	os_timer_arm(&timer,1000,1);

}
void user_rf_pre_init()
{

}
