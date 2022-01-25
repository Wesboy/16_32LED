#ifndef __W__MAX7219_H__
#define __W__MAX7219_H__




#define CS 15 // Pin cs  (SPI)
#define anzMAX 8


void *max7219_init(unsigned int *len); //all MAX7219 init
void max7219_set_brightness(unsigned short br); //brightness MAX7219
void updateMax7219Data(unsigned short **buff);
void refresh_display(); //take info into hwFrameBuff

#endif