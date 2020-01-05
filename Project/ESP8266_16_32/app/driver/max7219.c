#include "max7219.h"
#include "gpio.h"

void ICACHE_FLASH_ATTR max7219_init(void){
	PIN_FUNC_SELECT(DIN_Pin_MUX,DIN_Pin_FUNC);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(DIN_Pin_NUM), HIGH);
	os_printf("DIN init success~~\n");

	PIN_FUNC_SELECT(CS_Pin_MUX,CS_Pin_FUNC);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(CS_Pin_NUM), HIGH);
	os_printf("CS init success~~\n");

	PIN_FUNC_SELECT(CLK_Pin_MUX,CLK_Pin_FUNC);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(CLK_Pin_NUM), HIGH);
	os_printf("CLK init success~~\n");

	//max7219_write(0x09,0x00);	//解码寄存器（地址:0x09,写0x00时，选ND，解码7~0数码管）
    //max7219_write(0x0a,0x01);	//亮度寄存器
    //max7219_write(0x0b,0x07);	//扫描寄存器（0x0_，_等于显示数码管的个数）
    //max7219_write(0x0c,0x01);	//省电寄存器（写0x00进入掉电模式，写0x01进入正常模式）
    //max7219_write(0x0f,0x00); 	//测试寄存器（写0x01进入测试模式，写0x00进入正常模式）
	uint8_t i;

	for(i = 0; i < 8; i++)
	{
	    max7219_write_cascade(i+1, 0x09,0x00);	//解码寄存器（地址:0x09,写0x00时，选ND，解码7~0数码管）
	    max7219_write_cascade(i+1, 0x0a,0x01);	//亮度寄存器
	    max7219_write_cascade(i+1, 0x0b,0x07);	//扫描寄存器（0x0_，_等于显示数码管的个数）
	    max7219_write_cascade(i+1, 0x0c,0x01);	//省电寄存器（写0x00进入掉电模式，写0x01进入正常模式）
	    max7219_write_cascade(i+1, 0x0f,0x00); 	//测试寄存器（写0x01进入测试模式，写0x00进入正常模式）

	}
}

void ICACHE_FLASH_ATTR DIN_SET(uint8 status){
    GPIO_OUTPUT_SET(GPIO_ID_PIN(DIN_Pin_NUM), status);
}

void ICACHE_FLASH_ATTR CS_SET(uint8 status){
    GPIO_OUTPUT_SET(GPIO_ID_PIN(CS_Pin_NUM), status);
}

void ICACHE_FLASH_ATTR CLK_SET(uint8 status){
    GPIO_OUTPUT_SET(GPIO_ID_PIN(CLK_Pin_NUM), status);
}

void ICACHE_FLASH_ATTR writebyte(char data)
{
	char i;
	for(i=0;i<8;i++)
	{
		CLK_SET(LOW);
		uint8 st = data&0x80;
		if(st==0){
			DIN_SET(LOW);
		}else{
			DIN_SET(HIGH);
		}
		data=data<<1;
		CLK_SET(HIGH);
	}
}

void ICACHE_FLASH_ATTR max7219_write(char address,char date)
{
	CS_SET(LOW);
	writebyte(address);
	writebyte(date);
	CS_SET(HIGH);
}

void ICACHE_FLASH_ATTR max7219_write_cascade(unsigned char pcs, char address,char date)
{
	unsigned char i;
	CS_SET(LOW);
	writebyte(address);
	writebyte(date);

	for(i = 1; i < pcs; i++)
	{
		CLK_SET(HIGH);
		writebyte(0);
		writebyte(0);
	}
	CS_SET(HIGH);
}

void ICACHE_FLASH_ATTR max7219_clear(void){
	char i;
	for(i=0;i<8;i++){
		max7219_write_cascade(1, i+1,0x00);
		max7219_write_cascade(2, i+1,0x00);
		max7219_write_cascade(3, i+1,0x00);
		max7219_write_cascade(4, i+1,0x00);
		max7219_write_cascade(5, i+1,0x00);
		max7219_write_cascade(6, i+1,0x00);
		max7219_write_cascade(7, i+1,0x00);
		max7219_write_cascade(8, i+1,0x00);
	}
}

void ICACHE_FLASH_ATTR max7219_full(void){
	char i;
	for(i=0;i<8;i++){
		max7219_write_cascade(1, i+1,0xff);
		max7219_write_cascade(2, i+1,0xff);
		max7219_write_cascade(3, i+1,0xff);
		max7219_write_cascade(4, i+1,0xff);
		max7219_write_cascade(5, i+1,0xff);
		max7219_write_cascade(6, i+1,0xff);
		max7219_write_cascade(7, i+1,0xff);
		max7219_write_cascade(8, i+1,0xff);
	}
}


void ICACHE_FLASH_ATTR max7219_setval(unsigned char pcs, unsigned char val){
	char i;
	for(i=0;i<8;i++){
		max7219_write_cascade(pcs, i+1,val);
	}
}

void ICACHE_FLASH_ATTR max7219_setBrightness(unsigned char val){
	char i;
	for(i=0;i<8;i++){
		max7219_write_cascade(1, 0x0a,val);
		max7219_write_cascade(2, 0x0a,val);
		max7219_write_cascade(3, 0x0a,val);
		max7219_write_cascade(4, 0x0a,val);
		max7219_write_cascade(5, 0x0a,val);
		max7219_write_cascade(6, 0x0a,val);
		max7219_write_cascade(7, 0x0a,val);
		max7219_write_cascade(8, 0x0a,val);
	}
}


