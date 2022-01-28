
#include "wTicker.h"

static unsigned int _sys_tick_count = 0;

void wTickInit(void)
{

}

void wTick1ms(void)
{
	do
	{
		_sys_tick_count++;
	} while (0 == _sys_tick_count);
}


unsigned int ReadUserTimer(unsigned int *Timer)
{
	return (_sys_tick_count - *Timer);
}

void ResetUserTimer(unsigned int *Timer)
{
	*Timer = _sys_tick_count;
}

void SetUserTimer(unsigned int *Timer, unsigned int T)
{
	*Timer = _sys_tick_count + T;
}