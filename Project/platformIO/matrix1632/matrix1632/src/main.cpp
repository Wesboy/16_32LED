#include <Arduino.h>
/*********************************************************************************************************
MatrixShow.c

some show interface
*********************************************************************************************************/

#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <time.h>

#include "matrixDisplay.h"
#include "wfont.h"
#include "wDS3231.h"

//char ssid[] = "chen159";                    // your network SSID (name)
//char pass[] = "134802..chun";                    // your network password
// char ssid[] = "YOUGU";                    // your network SSID (name)
// char pass[] = "ygzhldcdz";                    // your network password

char ssid[] = "Kkkk";     // your network SSID (name)
char pass[] = "88888888"; // your network password

unsigned int z_PosX = 0;
unsigned int d_PosX = 0;
bool f_tckr1s = false;
bool f_tckr50ms = false;
unsigned long epoch = 0;
unsigned int localPort = 2390; // local port to listen for UDP packets
const char *ntpServerName = "time1.aliyun.com";
const int NTP_PACKET_SIZE = 48;     // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
IPAddress timeServerIP;

// The object for the Ticker
Ticker tckr;
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

//months
char M_arr[12][5] = {{' ', 'J', 'A', 'N', ' '}, {' ', 'F', 'E', 'B', ' '}, {' ', 'M', 'A', 'R', ' '}, {' ', 'A', 'P', 'R', ' '}, {' ', 'M', 'A', 'Y', ' '}, {' ', 'J', 'U', 'N', ' '}, {' ', 'J', 'U', 'L', ' '}, {' ', 'A', 'U', 'G', ' '}, {' ', 'S', 'E', 'P', ' '}, {' ', 'O', 'C', 'T', ' '}, {' ', 'N', 'O', 'V', ' '}, {' ', 'D', 'E', 'C', ' '}};
//days
char WT_arr[7][4] = {{'S', 'U', 'N', ' '}, {'M', 'O', 'N', ' '}, {'T', 'U', 'E', ' '}, {'W', 'E', 'D', ' '}, {'T', 'H', 'U', ' '}, {'F', 'R', 'I', ' '}, {'S', 'A', 'T', ' '}};



char tWifiTip[] = "WiFi";
char tWifiReadyTip[] = "Ready";
char tOKTip[] = "OK!!";
char tErrTip[] = "Err!!";
char tRTCTip[] = "RTC!";

//**************************************************************************************************

bool autoConfig()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass); // 默认连接保存的WIFI


    clear_Display();
    showText(tWifiTip, strlen(tWifiTip), 57, 0);
    showText(tWifiReadyTip, strlen(tWifiReadyTip), 29, 0);
    updatedisplay();

    for (int i = 0; i < 15; i++)
    {

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("AutoConfig Success");
            Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
            Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());

            clear_Display();
            showText(tOKTip, strlen(tOKTip), 25, 0);
            updatedisplay();

            Serial.println("WiFi connected");
            Serial.println(WiFi.localIP());
            Serial.println("Starting UDP");
            udp.begin(localPort);
            Serial.print("Local port: ");
            Serial.println(udp.localPort());
            return true;
        }
        else
        {
            delay(1000);
            Serial.print("AutoConfig Waiting......");
            Serial.print(digitalRead(0));
            Serial.println(WiFi.status());
            if(digitalRead(0) == 0)
            {
                clear_Display();
                showText(tRTCTip, strlen(tRTCTip), 25, 0);
                updatedisplay();
                return true;
            }
        }
    }
    clear_Display();
    showText(tErrTip, strlen(tErrTip), 25, 0);
    updatedisplay();
    delay(1000);
    Serial.println("AutoConfig Faild!");
    return false;
}
//**************************************************************************************************
void smartConfig()
{
    int i = 0;

    WiFi.mode(WIFI_STA);
    Serial.println("\r\nWait for Smartconfig");
    WiFi.beginSmartConfig();
    for (i = 0; i < 20; i++)
    {
        Serial.print(".");
        if (WiFi.smartConfigDone())
        {
            clear_Display();
            showText(tOKTip, strlen(tOKTip), 25, 0);
            updatedisplay();
            Serial.println("SmartConfig Success");
            Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
            Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
            WiFi.setAutoConnect(true); // 设置自动连接

            Serial.println("WiFi connected");
            Serial.println(WiFi.localIP());
            Serial.println("Starting UDP");
            udp.begin(localPort);
            Serial.print("Local port: ");
            Serial.println(udp.localPort());
            delay(1000);
            ESP.restart();
            break;
        }
        if(digitalRead(0) == 0)
        {
            i = 20;
        }
        clear_Display();
        showText("S", 1, 29, 0);
        showText("-", 1, 23, -1);
        showText("con", 3, 17, 0);
        updatedisplay();
        delay(1000);
    }
    if (i > 18)
    {
        clear_Display();
        showText(tRTCTip, strlen(tRTCTip), 25, 0);
        updatedisplay();
        delay(1000);
        Serial.println("SmartConfig Faild!");
        Serial.println("Clock use RTC!");
    }
}
//**************************************************************************************************
tm *connectNTP()
{ //if response from NTP was succesfull return *tm else return a nullpointer
    WiFi.hostByName(ntpServerName, timeServerIP);
    Serial.println(timeServerIP);
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
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
    delay(1000); // wait to see if a reply is available
    int cb = udp.parsePacket();
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
    epoch = secsSince1900 - seventyYears + 3600 * 8 + 2; //+2000ms Verarbeitungszeit
    //epoch=epoch-3600*6; // difference -6h = -6* 3600 sec)
    time_t t;
    t = epoch;
    tm *ftt;
    ftt = localtime(&t);
    Serial.println(epoch);
    Serial.println(asctime(ftt));
    if (cb == 48)
        return (ftt);
    else
        return (NULL);
}


//**************************************************************************************************
void timer50ms()
{
    static unsigned int cnt50ms = 0;
    f_tckr50ms = true;
    cnt50ms++;
    if (cnt50ms == 20)
    {
        f_tckr1s = true; // 1 sec
        cnt50ms = 0;
    }
}

//**************************************************************************************************
//
//The setup function is called once at startup of the sketch
void setup()
{
    // Add your initialization code here
    tm *tt;
    Serial.begin(115200);
    helpArr_init();
    matrixDisplayInit();
    rtc_init();
    clear_Display();
    tckr.attach(0.05, timer50ms); // every 50 msec
    pinMode(0, INPUT);
    //////////////////////////////////
    if (!autoConfig())
    {
        smartConfig();
    }
    ///////////////////////////////////
    tt = connectNTP();
    if (tt != NULL)
        rtc_set(tt);
    else
        Serial.println("no timepacket received");

    clear_Display();
}


//**************************************************************************************************
// The loop function is called in an endless loop
void loop()
{
    //Add your repeated code here
    struct tm mPerviousTime;
    struct tm mPresentTime;

    signed int x = 0; //x1,x2;
    signed int y = 0, y1 = 0, y2 = 0, y3 = 0;
    bool updown = false;
    bool f_scrollend_y = false;
    unsigned int f_scroll_x = false;

    //滚动单位
    unsigned char mVarietyType = 0;

    z_PosX = MAXPOSX;
    d_PosX = MAXPOSX;

    updown = true;
    if (updown == false)
    {
        y2 = -9;
        y1 = 8;
    }
    if (updown == true)
    { //scroll  up to down
        y2 = 8;
        y1 = -8;
    }
    while (true)
    {
        yield();
        if (mPresentTime.tm_hour == 0 && mPresentTime.tm_min == 20 && mPresentTime.tm_sec == 0) //syncronisize RTC every tm_mday 00:20:00
        {
            clear_Display();
            delay(500);
            ESP.restart();
        }

        //修改逻辑200ms读取一次rtc时钟，判断秒数变化再做更新
        if (f_tckr1s == true) // flag 1sek
        {
            rtc2mez(&mPresentTime);

            mVarietyType = 0;
            if(mPerviousTime.tm_sec != mPresentTime.tm_sec)
            {
                mPerviousTime.tm_sec = mPresentTime.tm_sec;
                
                mVarietyType |= 1;
                if(mPerviousTime.tm_sec%10 == 0)
                    mVarietyType |= 0x2;
                
            }

            if(mPerviousTime.tm_min != mPresentTime.tm_min)
            {
                mPerviousTime.tm_min = mPresentTime.tm_min;
                mVarietyType |= 0x4;
                if(mPerviousTime.tm_min%10 == 0)
                    mVarietyType |= 0x8;
            }

            
            if(mPerviousTime.tm_hour != mPresentTime.tm_hour)
            {
                mPerviousTime.tm_hour = mPresentTime.tm_hour;
                mVarietyType |= 0x10;
                if(mPerviousTime.tm_hour%10 == 0)
                    mVarietyType |= 0x20;
            }

            y = y2; //scroll updown
            f_tckr1s = false;

            // if (mPresentTime.tm_sec == 45)
            //     f_scroll_x = true; //滚动开关
        }                          // end 1s
        if (f_tckr50ms == true)
        {
            f_tckr50ms = false;
            if (f_scroll_x == true)
            {
                // z_PosX++;
                d_PosX++;
                if (d_PosX == 101)
                    z_PosX = 0;
                if (z_PosX == MAXPOSX)
                {
                    f_scroll_x = false;
                    d_PosX = -8;
                }
            }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            char22Arr(48 + mPresentTime.tm_mday % 10, z_PosX - 59, 0);
            char22Arr(48 + mPresentTime.tm_mday / 10, z_PosX - 55, 0);
            char22Arr(48 + mPresentTime.tm_mon % 10, z_PosX - 51, 0);
            char22Arr(48 + mPresentTime.tm_mon / 10, z_PosX - 47, 0);
            char22Arr(48 + mPresentTime.tm_year % 10, z_PosX - 43, 0);
            char22Arr(48 + mPresentTime.tm_year / 10, z_PosX - 39, 0);
            char22Arr('0', z_PosX - 35, 0);
            char22Arr('2', z_PosX - 31, 0); //year

            // char2Arr(' ', d_PosX+5, 0);        //tm_mday of the week
            // char2Arr(WT_arr[MEZ.WT][0], d_PosX - 1, 0);        //tm_mday of the week
            // char2Arr(WT_arr[MEZ.WT][1], d_PosX - 7, 0);
            // char2Arr(WT_arr[MEZ.WT][2], d_PosX - 13, 0);
            // char2Arr(WT_arr[MEZ.WT][3], d_PosX - 19, 0);
            // char2Arr(48 + MEZ.tag2, d_PosX - 24, 0);           //tm_mday
            // char2Arr(48 + MEZ.tag1, d_PosX - 30, 0);
            // char2Arr(M_arr[MEZ.mon12 - 1][0], d_PosX - 39, 0); //tm_mon
            // char2Arr(M_arr[MEZ.mon12 - 1][1], d_PosX - 43, 0);
            // char2Arr(M_arr[MEZ.mon12 - 1][2], d_PosX - 49, 0);
            // char2Arr(M_arr[MEZ.mon12 - 1][3], d_PosX - 55, 0);
            // char2Arr(M_arr[MEZ.mon12 - 1][4], d_PosX - 61, 0);
            // char2Arr('2', d_PosX - 68, 0);                     //year
            // char2Arr('0', d_PosX - 74, 0);
            // char2Arr(48 + MEZ.jahr2, d_PosX - 80, 0);
            // char2Arr(48 + MEZ.jahr1, d_PosX - 86, 0);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if((mVarietyType&0x1) == 0x1)
            {
                //秒个位变化
                if (updown == 1)
                    y--;
                else
                    y++;
                y3 = y;
                if (y3 > 0)
                {
                    y3 = 0;
                }
                char22Arr(48 + mPresentTime.tm_sec%10, z_PosX - 27, y3);
                char22Arr(48 + mPerviousTime.tm_sec%10, z_PosX - 27, y + y1);
                if (y == 0)
                {
                    mVarietyType &= 0xFE;
                    f_scrollend_y = true;
                }
            }
            else
                char22Arr(48 + mPresentTime.tm_sec%10, z_PosX - 27, 0);

            if((mVarietyType&0x2) == 0x2) //秒十位变化
            {        
                char22Arr(48 + mPresentTime.tm_sec/10, z_PosX - 23, y3);
                char22Arr(48 + mPerviousTime.tm_sec/10, z_PosX - 23, y + y1);
                if (y == 0)
                {
                    mVarietyType &= 0xFD;
                }
            }
            else
            {
                char22Arr(48 + mPresentTime.tm_sec/10, z_PosX - 23, 0);
            }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    
            if((mVarietyType&0x4) == 0x4) //分个位变化
            {
                char2Arr(48 + mPresentTime.tm_min%10, z_PosX - 18, y);
                char2Arr(48 + mPerviousTime.tm_min%10, z_PosX - 18, y + y1);
                if (y == 0)
                    mVarietyType &= 0xFB;
            }
            else
                char2Arr(48 + mPresentTime.tm_min%10, z_PosX - 18, 0);

            if((mVarietyType&0x8) == 0x8) //分十位变化
            {
                char2Arr(48 + mPresentTime.tm_min/10, z_PosX - 13, y);
                char2Arr(48 + mPerviousTime.tm_min/10, z_PosX - 13, y + y1);
                if (y == 0)
                    mVarietyType &= 0xF7;
            }
            else
                char2Arr(48 + mPresentTime.tm_min/10, z_PosX - 13, 0);

            char2Arr(':', z_PosX - 10 + x, 0);

            if((mVarietyType&0x10) == 0x10) //时个位变化
            {
                char2Arr(48 + mPresentTime.tm_hour%10, z_PosX - 4, y);
                char2Arr(48 + mPerviousTime.tm_hour%10, z_PosX - 4, y + y1);
                if (y == 0)
                    mVarietyType &= 0xEF;
            }
            else
                char2Arr(48 + mPresentTime.tm_hour%10, z_PosX - 4, 0);

            if((mVarietyType&0x20) == 0x20) //时十位变化
            {
                char2Arr(48 + mPresentTime.tm_hour/10, z_PosX + 1, y);
                char2Arr(48 + mPerviousTime.tm_hour/10, z_PosX + 1, y + y1);
                if (y == 0)
                    mVarietyType &= 0xDF;
            }
            else
                char2Arr(48 + mPresentTime.tm_hour/10, z_PosX + 1, 0);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            updatedisplay(); //alle 50ms
            if (f_scrollend_y == true)
            {
                f_scrollend_y = false;
            }
        } //end 50ms
        if (y == 0)
        {
            // do something else
        }
    } //end while(true)
    //this section can not be reached
}