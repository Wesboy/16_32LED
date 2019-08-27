/**
 * Copyright 
 * file
 * author
 * date
 * version
 */
#ifndef __HC595__H__
#define __HC595__H__

#include "main.h"

#define HC595_0				0
#define HC595_1				1

#define HC595_0_NUM		4
#define HC595_0_BIT		(HC595_0_NUM*8)
#define HC595_1_NUM		2
#define HC595_1_BIT		(HC595_1_NUM*8)

#define HC595_0_PORT				GPIOC

#define HC595_0_MR_PIN      GPIO_PIN_5
#define HC595_0_OE_PIN      GPIO_PIN_2
#define HC595_0_SHCP_PIN	  GPIO_PIN_4
#define HC595_0_STCP_PIN	  GPIO_PIN_3
#define HC595_0_DATA_PIN	  GPIO_PIN_1

#define HC595_1_MR_PIN      GPIO_PIN_0
#define HC595_1_OE_PIN      GPIO_PIN_13
#define HC595_1_SHCP_PIN	  GPIO_PIN_15
#define HC595_1_STCP_PIN	  GPIO_PIN_14
#define HC595_1_DATA_PIN	  GPIO_PIN_12

/**************************************************/
#define HC595_0_1        0x00000001
#define HC595_0_2       				0x00000002
#define HC595_0_3				0x00000004
#define HC595_0_4					0x00000008
#define HC595_0_5         0x00000010
#define HC595_0_6					0x00000020
#define HC595_0_7					      0x00000040
#define HC595_0_8         0x00000080

#define HC595_0_9         0x00000100
#define HC595_0_10        0x00000200
#define HC595_0_11          0x00000400
#define HC595_0_12           0x00000800
#define HC595_0_13           0x00001000
#define HC595_0_14         0x00002000
#define HC595_0_15        0x00004000
#define HC595_0_16            0x00008000

#define HC595_0_17            0x00010000
#define HC595_0_18           0x00020000
#define HC595_0_19             0x00040000
#define HC595_0_20             0x00080000
#define HC595_0_21           0x00100000
#define HC595_0_22         0x00200000
#define HC595_0_23   0x00400000
#define HC595_0_24   0x00800000

#define HC595_0_25           0x01000000
#define HC595_0_26            0x02000000
#define HC595_0_27         0x04000000
#define HC595_0_28            0x08000000
#define HC595_0_29           0x10000000
#define HC595_0_30         0x20000000
#define HC595_0_31       0x40000000
#define HC595_0_32     0x80000000

static void DelayTime(void);
static void HC595_shcp(uint8_t hc595_id, uint8_t val);
static void HC595_stcp(uint8_t hc595_id, uint8_t val);
static void HC595_data(uint8_t hc595_id, uint8_t val);
static void HC595_cs(uint8_t hc595_id);
static void HC595_Send(uint8_t hc595_id, uint32_t val);

extern void ExtHC595_Init(void);
extern void ExtHC595_Send(uint8_t hc595_id, uint32_t val);

#endif
