#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"

#include "os_type.h"
/**
 *******************************************************************************
 * @brief       ‰≥ˆ“˝Ω≈≈‰÷√
 *******************************************************************************
 */

#define CS_Pin_NUM		15
#define CS_Pin_FUNC		FUNC_GPIO15
#define CS_Pin_MUX		PERIPHS_IO_MUX_MTDO_U

#define DIN_Pin_NUM		13
#define DIN_Pin_FUNC	FUNC_GPIO13
#define DIN_Pin_MUX		PERIPHS_IO_MUX_MTCK_U

#define CLK_Pin_NUM		14
#define CLK_Pin_FUNC	FUNC_GPIO14
#define CLK_Pin_MUX		PERIPHS_IO_MUX_MTMS_U



#define HIGH 1
#define LOW 0


void max7219_init(void);

void DIN_SET(uint8 status);
void CS_SET(uint8 status);
void CLK_SET(uint8 status);
void writebyte(char data);
void max7219_write(char address,char date);
void max7219_write_cascade(unsigned char pcs, char address,char date);
void set_max7219_data(char line,uint32_t data0, uint32_t data1);
void max7219_setBrightness(unsigned char val);
void max7219_clear(void);






