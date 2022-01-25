#include "wDS3231.h"
#include <Wire.h>


#define DS3231_SDA 5 // Pin sda (I2C)
#define DS3231_SCL 4 // Pin scl (I2C)


unsigned char rtc_Read(unsigned char regaddress);
void rtc_Write(unsigned char regaddress, unsigned char value);


//**************************************************************************************************
void rtc_init(void)
{
    Wire.begin(DS3231_SDA, DS3231_SCL);
    rtc_Write(CONTROLREG, 0x00);
}
//**************************************************************************************************
// BCD Code
//**************************************************************************************************
static unsigned char dec2bcd(unsigned char x)
{ //value 0...99
    unsigned char z, e, r;
    e = x % 10;
    z = x / 10;
    z = z << 4;
    r = e | z;
    return (r);
}

static unsigned char bcd2dec(unsigned char x)
{ //value 0...99
    int z, e;
    e = x & 0x0F;
    z = x & 0xF0;
    z = z >> 4;
    z = z * 10;
    return (z + e);
}
//**************************************************************************************************
// RTC I2C Code
//**************************************************************************************************
unsigned char rtc_Read(unsigned char regaddress)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(regaddress);
    Wire.endTransmission();
    Wire.requestFrom((unsigned char)DS3231_ADDRESS, (unsigned char)1);
    return (Wire.read());
}
void rtc_Write(unsigned char regaddress, unsigned char value)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(regaddress);
    Wire.write(value);
    Wire.endTransmission();
}
//**************************************************************************************************
unsigned char rtc_second()
{
    return (bcd2dec(rtc_Read(SECONDREG)));
}
unsigned char rtc_minute()
{
    return (bcd2dec(rtc_Read(MINUTEREG)));
}
unsigned char rtc_stunde()
{
    return (bcd2dec(rtc_Read(HOURREG)));
}
unsigned char rtc_wochentag()
{
    return (bcd2dec(rtc_Read(WTREG)));
}
unsigned char rtc_tag()
{
    return (bcd2dec(rtc_Read(DATEREG)));
}
unsigned char rtc_monat()
{
    return (bcd2dec(rtc_Read(MONTHREG)));
}
unsigned char rtc_jahr()
{
    return (bcd2dec(rtc_Read(YEARREG)));
}
void rtc_second(unsigned char sek)
{
    rtc_Write(SECONDREG, (dec2bcd(sek)));
}
void rtc_minute(unsigned char min)
{
    rtc_Write(MINUTEREG, (dec2bcd(min)));
}
void rtc_stunde(unsigned char std)
{
    rtc_Write(HOURREG, (dec2bcd(std)));
}
void rtc_wochentag(unsigned char wt)
{
    rtc_Write(WTREG, (dec2bcd(wt)));
}
void rtc_tag(unsigned char tag)
{
    rtc_Write(DATEREG, (dec2bcd(tag)));
}
void rtc_monat(unsigned char mon)
{
    rtc_Write(MONTHREG, (dec2bcd(mon)));
}
void rtc_jahr(unsigned char jahr)
{
    rtc_Write(YEARREG, (dec2bcd(jahr)));
}
//**************************************************************************************************
void rtc_set(tm *tt)
{
    rtc_second((unsigned char)tt->tm_sec);
    rtc_minute((unsigned char)tt->tm_min);
    rtc_stunde((unsigned char)tt->tm_hour);
    rtc_tag((unsigned char)tt->tm_mday);
    rtc_monat((unsigned char)tt->tm_mon + 1);
    rtc_jahr((unsigned char)tt->tm_year - 100);
    if (tt->tm_wday == 0)
    {
        rtc_wochentag(7);
    }
    else
        rtc_wochentag((unsigned char)tt->tm_wday);
}
//**************************************************************************************************
float rtc_temp()
{
    float t = 0.0;
    unsigned char lowByte = 0;
    signed char highByte = 0;
    
    lowByte = rtc_Read(TEMPLSBREG);
    highByte = rtc_Read(TEMPMSBREG);
    lowByte >>= 6;
    lowByte &= 0x03;
    t = ((float)lowByte);
    t *= 0.25;
    t += highByte;
    return (t); // return temp value
}
//**************************************************************************************************
void rtc2mez(tm *getTime)
{
    unsigned short Jahr, Tag, Monat, WoTag, Stunde, Minute, Sekunde;

    getTime->tm_year = rtc_jahr(); //年
    if (getTime->tm_year > 99)
        getTime->tm_year = 0;
    getTime->tm_mon = rtc_monat(); //月
    if (getTime->tm_mon > 12)
        getTime->tm_mon = 0;
    getTime->tm_mday = rtc_tag(); //天
    if (getTime->tm_mday > 31)
        getTime->tm_mday = 0;
    getTime->tm_wday = rtc_wochentag(); //So=0, Mo=1, Di=2 ...
    if (getTime->tm_wday == 7)
        getTime->tm_wday = 0;
    getTime->tm_hour = rtc_stunde(); //小时
    if (getTime->tm_hour > 23)
        getTime->tm_hour = 0;
    getTime->tm_min = rtc_minute(); //分钟
    if (getTime->tm_min > 59)
        getTime->tm_min = 0;
    getTime->tm_sec = rtc_second(); //秒
    if (getTime->tm_sec > 59)
        getTime->tm_sec = 0;

}