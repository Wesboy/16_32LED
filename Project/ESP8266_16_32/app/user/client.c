#include "client.h"
#include "iconv.h"
#include "stdio.h"
#include "string.h"
#include "myTime.h"

MT *pTime;

void decodeTime(char *pdata, unsigned short len)
{
	unsigned char strtmp[] = "sysTime1";
	unsigned short bfind = 0;
	unsigned short i, j;
	pTime = GetTime();
	for(i = 0, j = 0; i < len; i++)
	{
		pTime->bFind = 0;
		if(pdata[i] == strtmp[j])
		{
			if(j < 7)
				j++;
			else
			{
				pTime->year_h = (pdata[i+4]-0x30)*10 + (pdata[i+5]-0x30);
				pTime->year_l = (pdata[i+6]-0x30)*10 + (pdata[i+7]-0x30);
				pTime->month = (pdata[i+8]-0x30)*10 + (pdata[i+9]-0x30);
				pTime->day = (pdata[i+10]-0x30)*10 + (pdata[i+11]-0x30);
				pTime->hour = (pdata[i+12]-0x30)*10 + (pdata[i+13]-0x30);
				pTime->minute = (pdata[i+14]-0x30)*10 + (pdata[i+15]-0x30);
				pTime->second = (pdata[i+16]-0x30)*10 + (pdata[i+17]-0x30);
				pTime->bFind = 1;

				os_printf("Systime1 = %s\r\n", &pdata[i]);
				os_printf("NetTime[%d] : %d:%d:%d\r\n", i, pTime->hour, pTime->minute, pTime->second);
				break;
			}
		}
		else
		{
			j = 0;
		}


	}
}

//成功接收到服务器返回数据函数
void ICACHE_FLASH_ATTR user_tcp_recv_cb(void *arg, char *pdata,
		unsigned short len) {
	uart0_sendStr("\r\n ----- 开始接受数据----- \r\n ");
	unsigned short length = strlen(pdata);
	uart0_tx_buffer(pdata, length);
	decodeTime(pdata, length);
	uart0_sendStr("\r\n -----结束接受数据-----  \r\n ");

}

//发送数据到服务器成功的回调函数
void ICACHE_FLASH_ATTR user_tcp_sent_cb(void *arg) {
	uart0_sendStr("发送数据成功！\r\n ");
}

//断开服务器成功的回调函数
void ICACHE_FLASH_ATTR user_tcp_discon_cb(void *arg) {
	uart0_sendStr("断开连接成功！\r\n ");
}

//连接失败的回调函数，err为错误代码
void ICACHE_FLASH_ATTR user_tcp_recon_cb(void *arg, sint8 err) {
	uart0_sendStr("连接错误，错误代码为%d\r\n", err);
	espconn_connect((struct espconn *) arg);
}

//成功连接到服务器的回调函数
void ICACHE_FLASH_ATTR user_tcp_connect_cb(void *arg) {
	struct espconn *pespconn = arg;
	espconn_regist_recvcb(pespconn, user_tcp_recv_cb);
	espconn_regist_sentcb(pespconn, user_tcp_sent_cb);
	espconn_regist_disconcb(pespconn, user_tcp_discon_cb);

	uart0_sendStr("\r\n ----- 请求数据开始----- \r\n");
	uart0_tx_buffer(buffer, strlen(buffer));
	uart0_sendStr("\r\n -----请求数据结束-----  \r\n");

	espconn_sent(pespconn, buffer, strlen(buffer));

}
void ICACHE_FLASH_ATTR my_station_init(struct ip_addr *remote_ip,
		struct ip_addr *local_ip, int remote_port) {
	//配置
	user_tcp_conn.type = ESPCONN_TCP;
	user_tcp_conn.state = ESPCONN_NONE;
	user_tcp_conn.proto.tcp = (esp_tcp *) os_zalloc(sizeof(esp_tcp));
	os_memcpy(user_tcp_conn.proto.tcp->local_ip, local_ip, 4);
	os_memcpy(user_tcp_conn.proto.tcp->remote_ip, remote_ip, 4);
	user_tcp_conn.proto.tcp->local_port = espconn_port();
	user_tcp_conn.proto.tcp->remote_port = remote_port;
	//注册
	espconn_regist_connectcb(&user_tcp_conn, user_tcp_connect_cb);
	espconn_regist_reconcb(&user_tcp_conn, user_tcp_recon_cb);
	//连接服务器
	espconn_connect(&user_tcp_conn);
}

