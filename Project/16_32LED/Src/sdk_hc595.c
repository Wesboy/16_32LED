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
  /*** ����3����STCP����һ�������أ���λ�Ĵ�����ȫ����������洢�Ĵ�������� ***/
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
    /*** ����1�������ݴ���DS���ţ����ж�Ҫ���͵�λ��1����0��Ȼ����595 DS���ӵ���������߻�͵�ƽ ***/
    if(data & 0x80000000)
		{
      HC595_data(1);
		}
    else
    {
      HC595_data(0);
    }
		
    /*** ����2����SHCPÿ����һ�������أ���ǰ��bit�ͱ�������λ�Ĵ��� ***/
    HC595_shcp(0);
    DelayTime();
    HC595_shcp(1);
    DelayTime();
		
    data <<= 1;	//����һλ������λ����λ�ƣ�ͨ��if(data & 0x80000000)�жϵ�λ�Ƿ�Ϊ1
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
