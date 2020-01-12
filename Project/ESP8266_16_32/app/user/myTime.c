#include "myTime.h"




static MT gmTime;


MT *GetTime(void)
{
	return &gmTime;
}
