#include <Ticker.h>

#include <dummy.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 char ssid[] = "chen159";                    // your network SSID (name)
 char pass[] = "134802..chun";                  // your network password
unsigned int localPort = 2390;                      // local port to listen for UDP packets
const char* ntpServerName = "ntp.ntsc.ac.cn";
const int NTP_PACKET_SIZE = 48;                     // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];                 // buffer to hold incoming and outgoing packets
IPAddress timeServerIP;                            // time.nist.gov NTP server address
tm *tt, ttm;
unsigned long epoch = 0;
const int timeZone = 8;
uint8_t bUpdateNTP = 1;


Ticker flipper;

enum eMode{
    InitMode,
    AutoConnMode,
    SmartConnMode,
    WiFiConnecdMode,
    TimeMode,
    MaxMode,
};

static eMode mWorkMode = InitMode;             //
static uint8_t bflushTime = 0;
static uint8_t iTickerTime = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#define PIN       15 // On Trinket or Gemma, suggest changing this to 1

#define RGB_LINE_MAX 8
#define RGB_ROW_MAX 32
#define NUMPIXELS RGB_LINE_MAX*RGB_ROW_MAX // Popular NeoPixel ring size

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

typedef struct WTIME
{
    /* data */
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
 uint8_t year_h;
  uint8_t year_l;
  uint8_t month;
  uint8_t day;
}wTime;


//rgb颜色值
uint32_t rgbvalue[NUMPIXELS] = {0};

uint8_t Matrix_5_7_Num[10][8] = {
    0x00, 0x0f, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0f, /*0*/
    0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, /*1*/
    0x00, 0x0f, 0x01, 0x01, 0x0f, 0x08, 0x08, 0x0f, /*2*/
    0x00, 0x0f, 0x01, 0x01, 0x0f, 0x01, 0x01, 0x0f, /*3*/
    0x00, 0x09, 0x09, 0x09, 0x0f, 0x01, 0x01, 0x01, /*4*/
    0x00, 0x0f, 0x08, 0x08, 0x0f, 0x01, 0x01, 0x0f, /*5*/
    0x00, 0x0f, 0x08, 0x08, 0x0f, 0x09, 0x09 ,0x0f, /*6*/
    0x00, 0x0f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /*7*/
    0x00, 0x0f, 0x09, 0x09, 0x0f, 0x09, 0x09, 0x0f, /*8*/
    0x00, 0x0f, 0x09, 0x09, 0x0f, 0x01, 0x01, 0x0f, /*9*/
};
uint8_t Matrix_3_5_Num[10][5] = {
    0x07, 0x05, 0x05, 0x05, 0x07, /*0*/
    0x02, 0x02, 0x02, 0x02, 0x02, /*1*/
    0x07, 0x01, 0x07, 0x04, 0x07, /*2*/
    0x07, 0x01, 0x07, 0x01, 0x07, /*3*/
    0x05, 0x05, 0x07, 0x01, 0x01, /*4*/
    0x07, 0x04, 0x07, 0x01, 0x07, /*5*/
    0x07, 0x04, 0x07, 0x05, 0x07, /*6*/
    0x07, 0x01, 0x01, 0x01, 0x01, /*7*/
    0x07, 0x05, 0x07, 0x05, 0x07, /*8*/
    0x07, 0x05, 0x07, 0x01, 0x07, /*9*/
};

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels


#define DS3231_ADDRESS 0X68
#define SECONDREG 0X00
#define MINUTEREG 0X01
#define HOURREG 0X02
#define WTREG 0X03 //WEEKDAY
#define DATEREG 0X04
#define MONTHREG 0X05
#define YEARREG 0X06
#define ALARM_MIN1SECREG 0X07
#define ALARM_MIN1MINREG 0X08
#define ALARM_MIN1HRREG 0X09
#define ALARM_MIN1DATEREG 0X0A
#define ALARM_MIN2MINREG 0X0B
#define ALARM_MIN2HRREG 0X0C
#define ALARM_MIN2DATEREG 0X0D
#define CONTROLREG 0X0E
#define STATUSREG 0X0F
#define AGEOFFSETREG 0X10
#define TEMPMSBREG 0X11
#define TEMPLSBREG 0X12
#define _24_HOUR_FORMAT 0
#define _12_HOUR_FORMAT 1
#define AM 0
#define PM 1


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


/**
 * 
 * line ： 具体行
 * row  ： 具体列
 * colorVal: 颜色代码   brg—— bit24 ：bit 0
 * iLight ： 亮度值     50 -- 1000
 * **/
int setRGB(uint32_t line, uint32_t row, uint32_t colorVal, int iLight)
{
    if(line > RGB_LINE_MAX-1 || row > RGB_ROW_MAX-1)
        return -1;

    uint32_t index = 0;

    //计算具体在哪个像素点
    //0 15-16 31-32 47-48 63-64 79-80 95-96 111-112 127-128 133-134 149-150 165-166 181-196 211-212 227-228 243-244 
    //1 14-17 30-33
//    if(line == 0)
    {
        if(row%2)
        {
            index = (row/2)*16 + 15-line;
        }
        else
        {
            index = (row/2)*16 + line;
        }
    }

    rgbvalue[index] = colorVal;
}


int flushRGB(void)
{
  //reset
  int i;

  pixels.clear(); // Set all pixel colors to 'off'
  for(i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(rgbvalue[i]>>16, (rgbvalue[i]>>8)&0xFF, rgbvalue[i]&0xFF));
  }
  pixels.show();   // Send the updated pixel colors to the hardware.
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



static wTime mTime;



void timeInit(void)
{
    memset(&mTime, 0, sizeof(wTime));
}



void flip() {

    
    struct tm mPresentTime;

    
    rtc2mez(&mPresentTime);
    mTime.second = mPresentTime.tm_sec;
    mTime.minute = mPresentTime.tm_min;
    mTime.hour = mPresentTime.tm_hour;
    mTime.day = mPresentTime.tm_mday;
    mTime.month = mPresentTime.tm_mon;
    mTime.year_h = mPresentTime.tm_year>>8;
    mTime.year_l = mPresentTime.tm_year&&0xFF;
    bflushTime = 1;
    if(mWorkMode == WiFiConnecdMode)
    {
        iTickerTime++;
    }
    

//   if(mTime.second >= 59)
//   {
//       mTime.second = 0;
//       if(mTime.minute >= 59)
//       {
//           mTime.minute = 0;
//           if(mTime.hour >= 23)
//           {
//               bUpdateNTP = 1;
//               mTime.hour = 0;
//           }
//           else
//           {
//               mTime.hour++;
//           }
//       }
//       else
//       {
//           mTime.minute++;
//       }
//   }
//   else
//   {
//       mTime.second++;
//     }
}

void timeHandle(void)
{
    uint8_t i, line = 0;
    uint32_t lineval = 0;
    static uint8_t CurSecond = 0xFF;
    uint32_t PixColor = 0xFFFFFF;
    static uint8_t CurSecDisplay = 0;
    if(CurSecond != mTime.second)
    {
      CurSecDisplay++;
      if(CurSecDisplay > 13)
      {
        CurSecDisplay = 0;
      }
      CurSecond = mTime.second;
     }
     
    Serial.print(mTime.hour);
    Serial.print(":");
    Serial.print(mTime.minute);
    Serial.print(":");
    Serial.println(mTime.second);
 

    if (mTime.hour == 0 && mTime.minute == 40 && mTime.second == 0) //syncronisize RTC every tm_mday 00:20:00
    {
        ESP.restart();
    }

    if(CurSecDisplay < 10)
    {
      //显示时间
      for(line = 0; line < 8; line++)
      {
          lineval = 0;
          lineval = Matrix_5_7_Num[mTime.hour/10][7-line]<<27;
          lineval |= Matrix_5_7_Num[mTime.hour%10][7-line]<<22;
          lineval |= Matrix_5_7_Num[mTime.minute/10][7-line]<<14;
          lineval |= Matrix_5_7_Num[mTime.minute%10][7-line]<<9;
          PixColor = 0xC638;
  
          if(line == 1 || line == 2 || line == 4 || line == 5)
          {
              lineval |= 0x180000;
          }
          if(line < 5)
          {
            
              lineval |= Matrix_3_5_Num[mTime.second/10][4 - line] << 5;
              lineval |= Matrix_3_5_Num[mTime.second%10][4 - line] << 1;
              PixColor = 0xC638;
          }
          
          for(i = 0; i < 32; i++)
          {
              if((lineval >> i) &0x1)
              {
                if((1<<i)&0x180000)
                {
                  setRGB(line,i, 0xFF0000, 200);
                }
                else
                  setRGB(line,i, PixColor, 200);
              }
              else
              {
                  setRGB(line,i, 0x0, 200);
              }
          }
      }
    }
    else if(CurSecDisplay >= 10 && CurSecDisplay < 13)
    {
 
      //显示日期
      for(line = 0; line < 8; line++)
      {
        lineval = 0;
        if(line < 5)
        {
          lineval = Matrix_3_5_Num[mTime.year_h/10][4 - line] << 28;
          lineval |= (Matrix_3_5_Num[mTime.year_h%10][4 - line] << 24);
          lineval |= (Matrix_3_5_Num[mTime.year_l/10][4 - line] << 20);
          lineval |= (Matrix_3_5_Num[mTime.year_l%10][4 - line] << 16);
          lineval |= (Matrix_3_5_Num[mTime.month/10][4 - line] << 12);
          lineval |= (Matrix_3_5_Num[mTime.month%10][4 - line] << 8);
          lineval |= (Matrix_3_5_Num[mTime.day/10][4 - line] << 4);
          lineval |= (Matrix_3_5_Num[mTime.day%10][4 - line] << 0);
          PixColor = 0xA678;
        }
  
       for(i = 0; i < 32; i++) 
        {
            if((lineval >> i) &0x1)
            {
                setRGB(line,i, PixColor, 200);
            }
            else
            {
                setRGB(line,i, 0x0, 200);
            }
        }
      }
    }
    
    flushRGB();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;


void connect_to_WiFi() {  // We start by connecting to a WiFi network
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  
}
tm* connectNTP() { //if response from NTP was succesfull return *tm else return a nullpointer
    WiFi.hostByName(ntpServerName, timeServerIP);
    Serial.println(timeServerIP);
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(timeServerIP, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
    delay(1000);                 // wait to see if a reply is available
    int cb = udp.parsePacket();
    if (!cb) {
    //解析包为空
      Serial.println("no any data!");
    } else {
      Serial.print("data len");
      Serial.println(cb);
      udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      // now convert NTP time into everyday time:
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      epoch = secsSince1900 - seventyYears + 3600*timeZone + 2; //+2000ms Verarbeitungszeit
      //epoch=epoch-3600*6; // difference -6h = -6* 3600 sec)
    }
    time_t t;
    t = epoch;
    tm* ltt;
    ltt = localtime(&t);
    if (cb == 48)
        return (ltt);
    else
        return (NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.setBrightness(10);
    flipper.attach_ms(200, flip);
    rtc_init();
    timeInit();
    mWorkMode = InitMode;
    bUpdateNTP = 1;
}

char bfirst = 0;

void loop() {

    while (true)
    {
        switch (mWorkMode)
        {
        case InitMode:
            //////////////////////////////////
            //connect net
            connect_to_WiFi();
            mWorkMode = WiFiConnecdMode;
            iTickerTime = 0;
            ///////////////////////////////////
            break;
        case AutoConnMode:
            // autoConfig();
            break;
        case SmartConnMode:
            // smartConfig();
            break;
        case WiFiConnecdMode:
            if(WiFi.status()== WL_CONNECTED)
            {
                tm *tt;
                tt = connectNTP();
                if (tt != NULL)
                {
                    rtc_set(tt);
                    mWorkMode = TimeMode;
                    Serial.println("NTP Time set to RTC");
                }
                else if(iTickerTime > 100) //超过10s
                {
                    iTickerTime = 0;
                    Serial.println("no timepacket received into time");
                    mWorkMode = TimeMode;
                }
                else
                {
                    
                    Serial.print("no timepacket received：");
                    Serial.println(iTickerTime);
                }
            }
            else if(iTickerTime > 100) //超过10s
            {
                iTickerTime = 0;
                Serial.println("no wifi not connect time out!!");
                mWorkMode = TimeMode;
            }
            break;

        case TimeMode:
            if(bflushTime == 1)
            {
                bflushTime = 0; 
                timeHandle();
            }   
        
        default:
            break;
        }
        
        yield();
    } //end while(true)

}
