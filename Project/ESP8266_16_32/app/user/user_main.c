
#include "driver/uart.h"  //����0��Ҫ��ͷ�ļ�
#include "osapi.h"  //����1��Ҫ��ͷ�ļ�
#include "user_interface.h" //WIFI������Ҫ��ͷ�ļ�
#include "gpio.h"  //�˿ڿ�����Ҫ��ͷ�ļ�
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
