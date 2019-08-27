#include "sdk_74LS138.h"


static void GPIO_WRITE(uint16_t LS138_Pin, uint8_t val)
{
  if(!val)
  {
    HAL_GPIO_WritePin(IO_74LS138_PORT, LS138_Pin, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(IO_74LS138_PORT, LS138_Pin, GPIO_PIN_SET);
  }
  
}

void MY_138_EN(uint8_t iSw)
{
    GPIO_WRITE(IO_74LS138_EN_PIN, 1);
    GPIO_WRITE(IO_74LS138_A_PIN, 1);
    GPIO_WRITE(IO_74LS138_C_PIN, 0);
    GPIO_WRITE(IO_74LS138_B_PIN, 1);
}

void MY_74LS138_Init(void)
{
    //GPIO_Init
//    GPIO_InitTypeDef  GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 

//    GPIO_InitStructure.GPIO_Pin = IO_74LS138_A_PIN | IO_74LS138_B_PIN| IO_74LS138_C_PIN| IO_74LS138_EN_PIN;				
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
//    GPIO_Init(IO_74LS138_A_PORT, &GPIO_InitStructure);					
//    GPIO_SetBits(IO_74LS138_A_PORT,IO_74LS138_A_PIN | IO_74LS138_B_PIN| IO_74LS138_C_PIN| IO_74LS138_EN_PIN);						 
// 					 
}

