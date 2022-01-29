#ifndef __MATRIX_DISPLAY__H____
#define __MATRIX_DISPLAY__H____

#include "wMAX7219.h"

#define MAXPOSX (anzMAX * 8 - 1)

void matrixDisplayInit(void);
void clear_Display(); //clear all
void char2Arr(unsigned short ch, int PosX, short PosY);
void char22Arr(unsigned short ch, int PosX, short PosY);
void showText(char *ch, unsigned int size, int startPosX, short startPosY);
void ScorllShowDate(char *text, unsigned int size, int startPosX, int endPosX, unsigned int delayMs);
void updatedisplay(void);
void helpArr_init();

#endif

