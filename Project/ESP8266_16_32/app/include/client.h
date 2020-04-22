/*
 * client.h
 *
 *  Created on: 2020Äê1ÔÂ11ÈÕ
 *      Author: Administrator
 */

#ifndef APP_INCLUDE_CLIENT_H_
#define APP_INCLUDE_CLIENT_H_

#include "user_main.h"
#include "espconn.h"
#include "mem.h"
char buffer[1024];
#define GET "GET /%s HTTP/1.1\r\nContent-Type: text/html;charset=utf-8\r\nAccept: */*\r\nHost: %s\r\nConnection: Keep-Alive\r\n\r\n"
#define POST "POST /%s HTTP/1.1\r\nAccept: */*\r\nContent-Length: %d\r\nContent-Type: application/json\r\nHost: %s\r\nConnection: Keep-Alive\r\n\r\n%s"
struct espconn user_tcp_conn;

void user_tcp_recv_cb(void *arg, char *pdata, unsigned short len);
void user_tcp_sent_cb(void *arg);
void user_tcp_discon_cb(void *arg);
void user_tcp_recon_cb(void *arg, sint8 err);
void user_tcp_connect_cb(void *arg);
void my_station_init(struct ip_addr *remote_ip, struct ip_addr *local_ip, int remote_port);

void my_station_pro(void);

#endif /* APP_INCLUDE_CLIENT_H_ */
