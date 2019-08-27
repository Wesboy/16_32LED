#ifndef __MY_138__H__
#define __MY_138__H__


#include "main.h"

//GPIOC_0    A
//GPIOC_1    B
//GPIOC_2    C
//GPIOC_3    EN


#define IO_74LS138_PORT GPIOC
#define IO_74LS138_A_PIN GPIO_PIN_0
#define IO_74LS138_B_PIN GPIO_PIN_1
#define IO_74LS138_C_PIN GPIO_PIN_2
#define IO_74LS138_EN_PIN GPIO_PIN_3

extern void MY_74LS138_Init(void);
extern void MY_138_EN(uint8_t iSw);


#endif 
