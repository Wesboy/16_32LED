//https://blog.csdn.net/k1ang/article/details/80397173
/*
 * Copyright reserved. <== Correct
 * file
 * author
 * date
 * version
 */
#include "sdk_hc595.h"

static void DelayTime(void)
{
  for (uint16_t t=5; t>0; t--);
}

static void GPIO_WRITE(uint16_t hc595_Pin, uint8_t val)
{
  if(!val)
  {
    HAL_GPIO_WritePin(HC595_PORT, hc595_Pin, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(HC595_PORT, hc595_Pin, GPIO_PIN_SET);
  }
  
}

static void HC595_shcp(uint8_t val)
{
    GPIO_WRITE(HC595_SHCP_PIN, val);
}

static void HC595_stcp(uint8_t val)
{
    GPIO_WRITE(HC595_STCP_PIN, val);
}

static void HC595_data(uint8_t val)
{
    GPIO_WRITE(HC595_DATA_PIN, val);
}

static void HC595_cs(void) 
{
  /*** 步骤3：让STCP产生一个上升沿，移位寄存器的全部数据移入存储寄存器并输出 ***/
  HC595_stcp(0);
  DelayTime();
  HC595_stcp(1);
}

static void HC595_Send(uint32_t val)
{	
  uint32_t data = val;
  uint32_t data_bit = HC595_BIT;

  for(uint32_t i=0; i<data_bit; i++)
  {
    /*** 步骤1：将数据传到DS引脚，先判断要发送的位是1还是0，然后让595 DS连接的引脚输出高或低电平 ***/
    if(data & 0x80000000)
		{
      HC595_data(1);
		}
    else
    {
      HC595_data(0);
    }
		
    /*** 步骤2：让SHCP每产生一个上升沿，当前的bit就被送入移位寄存器 ***/
    HC595_shcp(0);
    DelayTime();
    HC595_shcp(1);
    DelayTime();
		
    data <<= 1;	//左移一位，将低位往高位移，通过if(data & 0x80000000)判断低位是否为1
  }
}

void ExtHC595_Send(uint32_t val)
{
  HC595_Send(val);
  HC595_cs();
}

void ExtHC595_Init(void)
{
//  GPIO_SetFunc(HC595_SHCP_PIN, 0);
//  GPIO_SetDir(HC595_SHCP_PIN, 1);
//  
//  GPIO_SetFunc(HC595_STCP_PIN, 0);
//  GPIO_SetDir(HC595_STCP_PIN, 1);
//  
//  GPIO_SetFunc(HC595_DATA_PIN, 0);
//  GPIO_SetDir(HC595_DATA_PIN, 1);
//  
//  GPIO_SetFunc(HC595_MR_PIN, 0);
//  GPIO_SetDir(HC595_MR_PIN, 1);
//  GPIO_WRITE(HC595_MR_PIN, 1);
//  
//  GPIO_SetFunc(HC595_OE_PIN, 0);
//  GPIO_SetDir(HC595_OE_PIN, 1);
//  GPIO_WRITE(HC595_OE_PIN, 0);
}
