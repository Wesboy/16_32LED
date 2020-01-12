/*
 * whttp.h
 *
 *  Created on: 2020Äê1ÔÂ11ÈÕ
 *      Author: Administrator
 */

#ifndef APP_INCLUDE_WHTTP_H_
#define APP_INCLUDE_WHTTP_H_



char host[32];
char filename[208];
unsigned short port;

void startHttpQuestByGET(char *URL);
void startHttpQuestByPOST(char *URL,char *method,char *postdata);

#endif /* APP_INCLUDE_WHTTP_H_ */
