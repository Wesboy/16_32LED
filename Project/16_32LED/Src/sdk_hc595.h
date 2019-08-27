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

#define HC595_NUM 4
#define HC595_BIT (HC595_NUM * 8)

#define HC595_PORT GPIOC

#define HC595_MR_PIN GPIO_PIN_8
#define HC595_OE_PIN GPIO_PIN_5
#define HC595_SHCP_PIN GPIO_PIN_7
#define HC595_STCP_PIN GPIO_PIN_6
#define HC595_DATA_PIN GPIO_PIN_4

/**************************************************/
#define HC595_1 0x00000001
#define HC595_2 0x00000002
#define HC595_3 0x00000004
#define HC595_4 0x00000008
#define HC595_5 0x00000010
#define HC595_6 0x00000020
#define HC595_7 0x00000040
#define HC595_8 0x00000080

#define HC595_9 0x00000100
#define HC595_10 0x00000200
#define HC595_11 0x00000400
#define HC595_12 0x00000800
#define HC595_13 0x00001000
#define HC595_14 0x00002000
#define HC595_15 0x00004000
#define HC595_16 0x00008000

#define HC595_17 0x00010000
#define HC595_18 0x00020000
#define HC595_19 0x00040000
#define HC595_20 0x00080000
#define HC595_21 0x00100000
#define HC595_22 0x00200000
#define HC595_23 0x00400000
#define HC595_24 0x00800000

#define HC595_25 0x01000000
#define HC595_26 0x02000000
#define HC595_27 0x04000000
#define HC595_28 0x08000000
#define HC595_29 0x10000000
#define HC595_30 0x20000000
#define HC595_31 0x40000000
#define HC595_32 0x80000000

static void DelayTime(void);
static void HC595_shcp(uint8_t val);
static void HC595_stcp(uint8_t val);
static void HC595_data(uint8_t val);
static void HC595_cs(void);
static void HC595_Send(uint32_t val);

extern void ExtHC595_Init(void);
extern void ExtHC595_Send(uint32_t val);

#endif
