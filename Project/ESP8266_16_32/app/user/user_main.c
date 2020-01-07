
#include "driver/uart.h"  //串口0需要的头文件
#include "osapi.h"  //串口1需要的头文件
#include "user_interface.h" //WIFI连接需要的头文件
#include "gpio.h"  //端口控制需要的头文件
#include "display_matrix.h"


os_timer_t timer;

void user_init()
{
	display_init();
	os_timer_disarm(&timer);
	os_timer_setfn(&timer,display_pro,NULL);
	os_timer_arm(&timer,500,1);

}
void user_rf_pre_init()
{

}
