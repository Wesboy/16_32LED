#ifndef __W_TICKER_H__
#define __W_TICKER_H__

void wTickInit(void);

void wTick1ms(void);
void SetUserTimer(unsigned int *Timer,unsigned int T);
void ResetUserTimer(unsigned int *Timer);
unsigned int ReadUserTimer(unsigned int *Timer);

#endif