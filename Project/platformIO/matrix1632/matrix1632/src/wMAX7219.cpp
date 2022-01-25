#include "wMAX7219.h"



#include <SPI.h>


static unsigned short (*hwFrameBuff)[8];


//*************************************************************************************************
const unsigned short InitArr[7][2] = {
    {0x0C, 0x00}, // display off
    {0x00, 0xFF}, // no LEDtest
    {0x09, 0x00}, // BCD off
    {0x0F, 0x00}, // normal operation
    {0x0B, 0x07}, // start display
    {0x0A, 0x04}, // brightness
    {0x0C, 0x01}  // display on
};
//**************************************************************************************************
void *max7219_init(unsigned int *len) //all MAX7219 init//all MAX7219 init
{
    *len = anzMAX*8;
    hwFrameBuff = (unsigned short (*)[8])malloc(sizeof(unsigned short)*(anzMAX*8));
    if(hwFrameBuff)
    {
        Serial.print("display hwFrameBuff: ok");
        SPI.begin();
        
        pinMode(CS, OUTPUT);
        digitalWrite(CS, HIGH);

        unsigned short i, j;
        for (i = 0; i < 7; i++)
        {
            digitalWrite(CS, LOW);
            delayMicroseconds(1);
            for (j = 0; j < anzMAX; j++)
            {
                SPI.write(InitArr[i][0]); //register
                SPI.write(InitArr[i][1]); //value
            }
            digitalWrite(CS, HIGH);
        }
    }

    return (void *)hwFrameBuff;
}
//**************************************************************************************************
void max7219_set_brightness(unsigned short br) //brightness MAX7219
{
    unsigned short j;
    if (br < 16)
    {
        digitalWrite(CS, LOW);
        delayMicroseconds(1);
        for (j = 0; j < anzMAX; j++)
        {
            SPI.write(0x0A); //register
            SPI.write(br);   //value
        }
        digitalWrite(CS, HIGH);
    }
}


//**************************************************************************************************
void refresh_display() //take info into hwFrameBuff
{
    unsigned short i, j;

#ifdef ROTATE_90
    rotate_90();
#endif

    for (i = 0; i < 8; i++) //8 rows
    {
        digitalWrite(CS, LOW);
        delayMicroseconds(1);
        for (j = 1; j <= anzMAX; j++)
        {
            SPI.write(i + 1); //current row

#ifdef REVERSE_HORIZONTAL
            SPI.setBitOrder(LSBFIRST); // bitorder for reverse columns
#endif

#ifdef REVERSE_VERTICAL
            SPI.write(hwFrameBuff[j - 1][7 - i]);
#else
            SPI.write(hwFrameBuff[j - 1][i]);
#endif

#ifdef REVERSE_HORIZONTAL
            SPI.setBitOrder(MSBFIRST); // reset bitorder
#endif
        }
        digitalWrite(CS, HIGH);
    }
}


//**************************************************************************************************
void updateMax7219Data(unsigned short **buff)
{
    // short temp = 0;
    // if(buff != NULL)
    { 
        // memcpy(hwFrameBuff, buff, sizeof(hwFrameBuff));
        // temp = hwFrameBuff[0][0];
        refresh_display(); //take info into hwFrameBuff
        // Serial.print("updateMax7219Data:");
        // Serial.print(temp);
        // Serial.println("ok");
        // temp = buff[0][0];
        // Serial.println(temp);
    }
}


//*********************************************W*****************************************************