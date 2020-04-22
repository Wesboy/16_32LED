
#include <display_matrix.h>
#include "uart.h"  //����0��Ҫ��ͷ�ļ�
#include "osapi.h"  //����1��Ҫ��ͷ�ļ�
#include "user_main.h" //WIFI������Ҫ��ͷ�ļ�
#include "gpio.h"  //�˿ڿ�����Ҫ��ͷ�ļ�


os_timer_t timer;
os_timer_t checkTimer_wifistate;

void Check_WifiState(void) {
	uint8 getState;
	getState = wifi_station_get_connect_status();

	//���״̬��ȷ��֤���Ѿ��ɹ����ӵ�·����
	if (getState == STATION_GOT_IP) {
		os_printf("WIFI���ӳɹ���");
		os_timer_disarm(&checkTimer_wifistate);
		os_timer_disarm(&connect_timer);

		uint8 status = wifi_station_get_connect_status();
		if (status == STATION_GOT_IP) {
			uart0_sendStr("WIFI���ӳɹ���");
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

	wifi_set_opmode(0x01); //����ΪSTATIONģʽ
	struct station_config stationConf;
	os_strcpy(stationConf.ssid, "FLY-DDNETWORK_guest");	  //�ĳ����Լ���   ·�������û���
	os_strcpy(stationConf.password, "flyaudio"); //�ĳ����Լ���   ·����������
	wifi_station_set_config(&stationConf); //����WiFi station�ӿ����ã������浽 flash
	wifi_station_connect(); //����·����

	os_timer_disarm(&checkTimer_wifistate); //ȡ����ʱ����ʱ
	os_timer_setfn(&checkTimer_wifistate, (os_timer_func_t *) Check_WifiState,
	NULL); //���ö�ʱ���ص�����
	os_timer_arm(&checkTimer_wifistate, 100, true); //������ʱ������λ������

	os_timer_disarm(&timer);
	os_timer_setfn(&timer,display_time_pro,NULL);
	os_timer_arm(&timer,1000,1);

}
void user_rf_pre_init()
{

}
