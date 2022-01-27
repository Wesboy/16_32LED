
#include "matrixDisplay.h"
#include "wfont.h"
#include <Arduino.h>

static unsigned short (*showFrameBuff)[8] = NULL;
static unsigned int len;

unsigned short helpArrMAX[anzMAX * 8];
unsigned short helpArrPos[anzMAX * 8];


void matrixDisplayInit(void)
{
    unsigned short temp; 
    showFrameBuff = (unsigned short (*)[8])max7219_init(&len);

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            temp = showFrameBuff[i][j];
            Serial.print(temp);

        }
    }
    max7219_set_brightness(3);
}

//**************************************************************************************************
void helpArr_init(void) //helperarray init
{
    unsigned short i, j, k;
    j = 0;
    k = 0;
    for (i = 0; i < anzMAX * 8; i++)
    {
        helpArrPos[i] = (1 << j); //bitmask
        helpArrMAX[i] = k;
        j++;
        if (j > 7)
        {
            j = 0;
            k++;
        }
    }
}


void rotate_90() // for Generic displays
{
    for (unsigned char k = anzMAX; k > 0; k--)
    {

        unsigned char i, j, m, imask, jmask;
        unsigned char tmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        for (i = 0, imask = 0x01; i < 8; i++, imask <<= 1)
        {
            for (j = 0, jmask = 0x01; j < 8; j++, jmask <<= 1)
            {
                if (showFrameBuff[k - 1][i] & jmask)
                {
                    tmp[j] |= imask;
                }
            }
        }
        for (m = 0; m < 8; m++)
        {
            showFrameBuff[k - 1][m] = tmp[m];
        }
    }
}



void clear_Display() //clear all
{
    unsigned short i, j;
    for (i = 0; i < 8; i++) //8 rows
    {
        for (j = anzMAX; j > 0; j--)
        {
            showFrameBuff[j - 1][i] = 0; //hwFrameBuff clear
        }
    }
    refresh_display();
}


void updatedisplay(void)
{
    refresh_display();
}


//**************************************************************************************************
void char2Arr(unsigned short ch, int PosX, short PosY)
{                                      //characters into arr
    int i, j, k, l, m, o1, o2, o3, o4; //in showFrameBuff
    PosX++;
    k = ch - 32;              //ASCII position in font
    if ((k >= 0) && (k < 96)) //character found in font?
    {
        o4 = font1[k][0]; //character width
        o3 = 1 << (o4 - 2);
        for (i = 0; i < o4; i++)
        {
            if (((PosX - i <= MAXPOSX) && (PosX - i >= 0)) && ((PosY > -8) && (PosY < 8))) //within matrix?
            {
                o1 = helpArrPos[PosX - i];
                o2 = helpArrMAX[PosX - i];
                for (j = 0; j < 8; j++)
                {
                    if (((PosY >= 0) && (PosY <= j)) || ((PosY < 0) && (j < PosY + 8))) //scroll vertical
                    {
                        l = font1[k][j + 1];
                        m = (l & (o3 >> i)); //e.g. o4=7  0zzzzz0, o4=4  0zz0
                        if (m > 0)
                            showFrameBuff[o2][j - PosY] = showFrameBuff[o2][j - PosY] | (o1); //set point
                        else
                            showFrameBuff[o2][j - PosY] = showFrameBuff[o2][j - PosY] & (~o1); //clear point
                    }
                }
            }
        }
    }
}

void char22Arr(unsigned short ch, int PosX, short PosY)
{                                      //characters into arr
    int i, j, k, l, m, o1, o2, o3, o4; //in showFrameBuff
    PosX++;
    k = ch - 32;              //ASCII position in font
    if ((k >= 0) && (k < 96)) //character found in font?
    {
        o4 = font2[k][0]; //character width
        o3 = 1 << (o4 - 2);
        for (i = 0; i < o4; i++)
        {
            if (((PosX - i <= MAXPOSX) && (PosX - i >= 0)) && ((PosY > -8) && (PosY < 8))) //within matrix?
            {
                o1 = helpArrPos[PosX - i];
                o2 = helpArrMAX[PosX - i];
                for (j = 0; j < 8; j++)
                {
                    if (((PosY >= 0) && (PosY <= j)) || ((PosY < 0) && (j < PosY + 8))) //scroll vertical
                    {
                        l = font2[k][j + 1];
                        m = (l & (o3 >> i)); //e.g. o4=7  0zzzzz0, o4=4  0zz0
                        if (m > 0)
                            showFrameBuff[o2][j - PosY] = showFrameBuff[o2][j - PosY] | (o1); //set point
                        else
                            showFrameBuff[o2][j - PosY] = showFrameBuff[o2][j - PosY] & (~o1); //clear point
                    }
                }
            }
        }
    }
}



//**************************************************************************************************
void showText(char *ch, unsigned int size, int startPosX, short startPosY)
{        
    int i = 0;
    if(size > 0)
    {
        for(i = 0; i < size; i++)
        {
            if((startPosX - i*6) > 0)
                char2Arr(ch[i] , startPosX - i*6, startPosY);
            else
                break;
        }
    }
}
//**************************************************************************************************

