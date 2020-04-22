
#include <display_matrix.h>
#include "uart.h"  //串口0需要的头文件
#include "osapi.h"  //串口1需要的头文件
#include "user_main.h" //WIFI连接需要的头文件
#include "gpio.h"  //端口控制需要的头文件


os_timer_t timer;
os_timer_t checkTimer_wifistate;

void Check_WifiState(void) {
	uint8 getState;
	getState = wifi_station_get_connect_status();

	//如果状态正确，证明已经成功连接到路由器
	if (getState == STATION_GOT_IP) {
		os_printf("WIFI连接成功！");
		os_timer_disarm(&checkTimer_wifistate);
		os_timer_disarm(&connect_timer);

		uint8 status = wifi_station_get_connect_status();
		if (status == STATION_GOT_IP) {
			uart0_sendStr("WIFI连接成功！");
			startHttpQuestByGET(
					//"https://api.seniverse.com/v3/weather/daily.json?key=rrpd2zmqkpwlsckt&location=guangzhou&language=en&unit=c&start=0&days=3");
					"http://quan.suning.com/getSysTime.do");
			return;
		}
	}

}

void user_init()
{
	display_init();

	wifi_set_opmode(0x01); //设置为STATION模式
	struct station_config stationConf;
	os_strcpy(stationConf.ssid, "FLY-DDNETWORK_guest");	  //改成你自己的   路由器的用户名
	os_strcpy(stationConf.password, "flyaudio"); //改成你自己的   路由器的密码
	wifi_station_set_config(&stationConf); //设置WiFi station接口配置，并保存到 flash
	wifi_station_connect(); //连接路由器

	os_timer_disarm(&checkTimer_wifistate); //取消定时器定时
	os_timer_setfn(&checkTimer_wifistate, (os_timer_func_t *) Check_WifiState,
	NULL); //设置定时器回调函数
	os_timer_arm(&checkTimer_wifistate, 100, true); //启动定时器，单位：毫秒

	os_timer_disarm(&timer);
	os_timer_setfn(&timer,display_time_pro,NULL);
	os_timer_arm(&timer,1000,1);

}
void user_rf_pre_init()
{

}
