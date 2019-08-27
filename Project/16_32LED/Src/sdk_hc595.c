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
    HAL_GPIO_WritePin(HC595_0_PORT, hc595_Pin, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(HC595_0_PORT, hc595_Pin, GPIO_PIN_SET);
  }
  
}

static void HC595_shcp(uint8_t hc595_id, uint8_t val)
{
  if(hc595_id == HC595_0)
  {
    GPIO_WRITE(HC595_0_SHCP_PIN, val);
  }
  else
  {
    GPIO_WRITE(HC595_1_SHCP_PIN, val);
  }
}

static void HC595_stcp(uint8_t hc595_id, uint8_t val)
{
  if(hc595_id == HC595_0)
  {
    GPIO_WRITE(HC595_0_STCP_PIN, val);
  }
  else
  {
    GPIO_WRITE(HC595_1_STCP_PIN, val);
  }
}

static void HC595_data(uint8_t hc595_id, uint8_t val)
{
  if(hc595_id == HC595_0)
  {
    GPIO_WRITE(HC595_0_DATA_PIN, val);
  }
  else
  {
    GPIO_WRITE(HC595_1_DATA_PIN, val);
  }
}

static void HC595_cs(uint8_t hc595_id) 
{
  /*** ����3����STCP����һ�������أ���λ�Ĵ�����ȫ����������洢�Ĵ�������� ***/
  HC595_stcp(hc595_id, 0);
  DelayTime();
  HC595_stcp(hc595_id, 1);
}

static void HC595_Send(uint8_t hc595_id, uint32_t val)
{	
  uint32_t data = (hc595_id == HC595_0)? val : (val << (32 - HC595_1_BIT));
  uint32_t data_bit = (hc595_id == HC595_0)? HC595_0_BIT : HC595_1_BIT;

  for(uint32_t i=0; i<data_bit; i++)
  {
    /*** ����1�������ݴ���DS���ţ����ж�Ҫ���͵�λ��1����0��Ȼ����595 DS���ӵ���������߻�͵�ƽ ***/
    if(data & 0x80000000)
		{
      HC595_data(hc595_id, 1);
		}
    else
    {
      HC595_data(hc595_id, 0);
    }
		
    /*** ����2����SHCPÿ����һ�������أ���ǰ��bit�ͱ�������λ�Ĵ��� ***/
    HC595_shcp(hc595_id, 0);
    DelayTime();
    HC595_shcp(hc595_id, 1);
    DelayTime();
		
    data <<= 1;	//����һλ������λ����λ�ƣ�ͨ��if(data & 0x80000000)�жϵ�λ�Ƿ�Ϊ1
  }
}

void ExtHC595_Send(uint8_t hc595_id, uint32_t val)
{
  HC595_Send(hc595_id, val);
  HC595_cs(hc595_id);
}

void ExtHC595_Init(void)
{
//  GPIO_SetFunc(HC595_0_SHCP_PIN, 0);
//  GPIO_SetDir(HC595_0_SHCP_PIN, 1);
//  
//  GPIO_SetFunc(HC595_0_STCP_PIN, 0);
//  GPIO_SetDir(HC595_0_STCP_PIN, 1);
//  
//  GPIO_SetFunc(HC595_0_DATA_PIN, 0);
//  GPIO_SetDir(HC595_0_DATA_PIN, 1);

//  GPIO_SetFunc(HC595_1_SHCP_PIN, 0);
//  GPIO_SetDir(HC595_1_SHCP_PIN, 1);
//  
//  GPIO_SetFunc(HC595_1_STCP_PIN, 0);
//  GPIO_SetDir(HC595_1_STCP_PIN, 1);
//  
//  GPIO_SetFunc(HC595_1_DATA_PIN, 0);
//  GPIO_SetDir(HC595_1_DATA_PIN, 1);
//  
//  GPIO_SetFunc(HC595_0_MR_PIN, 0);
//  GPIO_SetDir(HC595_0_MR_PIN, 1);
//  GPIO_WRITE(HC595_0_MR_PIN, 1);
//  
//  GPIO_SetFunc(HC595_0_OE_PIN, 0);
//  GPIO_SetDir(HC595_0_OE_PIN, 1);
//  GPIO_WRITE(HC595_0_OE_PIN, 0);

//  GPIO_SetFunc(HC595_1_MR_PIN, 0);
//  GPIO_SetDir(HC595_1_MR_PIN, 1);
//  GPIO_WRITE(HC595_1_MR_PIN, 1);
//  
//  GPIO_SetFunc(HC595_1_OE_PIN, 0);
//  GPIO_SetDir(HC595_1_OE_PIN, 1);  
//  GPIO_WRITE(HC595_1_OE_PIN, 0);
}
