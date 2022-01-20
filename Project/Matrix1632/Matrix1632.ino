/*********************************************************************************************************
MatrixShow.c

some show interface



*********************************************************************************************************/

#include <SPI.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <time.h>

#define SDA 5 // Pin sda (I2C)
#define SCL 4 // Pin scl (I2C)
#define CS 15 // Pin cs  (SPI)
#define anzMAX 8

//char ssid[] = "chen159";                    // your network SSID (name)
//char pass[] = "134802..chun";                    // your network password
// char ssid[] = "YOUGU";                    // your network SSID (name)
// char pass[] = "ygzhldcdz";                    // your network password

char ssid[] = "Kkkk";     // your network SSID (name)
char pass[] = "88888888"; // your network password

unsigned short maxPosX = anzMAX * 8 - 1;
unsigned short LEDarr[anzMAX][8];
unsigned short helpArrMAX[anzMAX * 8];
unsigned short helpArrPos[anzMAX * 8];
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
tm *gtt, ttm;

const unsigned char DS3231_ADDRESS = 0x68;
const unsigned char secondREG = 0x00;
const unsigned char minuteREG = 0x01;
const unsigned char hourREG = 0x02;
const unsigned char WTREG = 0x03; //weekday
const unsigned char dateREG = 0x04;
const unsigned char monthREG = 0x05;
const unsigned char yearREG = 0x06;
const unsigned char alarm_min1secREG = 0x07;
const unsigned char alarm_min1minREG = 0x08;
const unsigned char alarm_min1hrREG = 0x09;
const unsigned char alarm_min1dateREG = 0x0A;
const unsigned char alarm_min2minREG = 0x0B;
const unsigned char alarm_min2hrREG = 0x0C;
const unsigned char alarm_min2dateREG = 0x0D;
const unsigned char controlREG = 0x0E;
const unsigned char statusREG = 0x0F;
const unsigned char ageoffsetREG = 0x10;
const unsigned char tempMSBREG = 0x11;
const unsigned char tempLSBREG = 0x12;
const unsigned char _24_hour_format = 0;
const unsigned char _12_hour_format = 1;
const unsigned char AM = 0;
const unsigned char PM = 1;

struct DateTime
{
    unsigned char second, minute, hour, week;
    unsigned short day, month, jahr;
} mTime;

// The object for the Ticker
Ticker tckr;
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

//months
char M_arr[12][5] = {{' ', 'J', 'A', 'N', ' '}, {' ', 'F', 'E', 'B', ' '}, {' ', 'M', 'A', 'R', ' '}, {' ', 'A', 'P', 'R', ' '}, {' ', 'M', 'A', 'Y', ' '}, {' ', 'J', 'U', 'N', ' '}, {' ', 'J', 'U', 'L', ' '}, {' ', 'A', 'U', 'G', ' '}, {' ', 'S', 'E', 'P', ' '}, {' ', 'O', 'C', 'T', ' '}, {' ', 'N', 'O', 'V', ' '}, {' ', 'D', 'E', 'C', ' '}};
//days
char WT_arr[7][4] = {{'S', 'U', 'N', ' '}, {'M', 'O', 'N', ' '}, {'T', 'U', 'E', ' '}, {'W', 'E', 'D', ' '}, {'T', 'H', 'U', ' '}, {'F', 'R', 'I', ' '}, {'S', 'A', 'T', ' '}};

// Zeichensatz 5x8 in einer 8x8 Matrix, 0,0 ist rechts oben
unsigned short const font1[96][9] = {
    {0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00},                                     // 0x20, Space
    {0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04, 0x00}, // 0x21, !
    {0x07, 0x09, 0x09, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x22, "
    {0x07, 0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a, 0x00}, // 0x23, #
    {0x07, 0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04, 0x00}, // 0x24, $
    {0x07, 0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13, 0x00}, // 0x25, %
    {0x07, 0x04, 0x0a, 0x0a, 0x0a, 0x15, 0x12, 0x0d, 0x00}, // 0x26, &
    {0x07, 0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x27, '
    {0x07, 0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02, 0x00}, // 0x28, (
    {0x07, 0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00}, // 0x29, )
    {0x07, 0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04, 0x00}, // 0x2a, *
    {0x07, 0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00, 0x00}, // 0x2b, +
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02}, // 0x2c, ,
    {0x07, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00}, // 0x2d, -
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00}, // 0x2e, .
    {0x07, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10, 0x00}, // 0x2f, /
    {0x07, 0x0F, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0F, 0x00}, // 0x30, 0
    {0x07, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00}, // 0x31, 1
    {0x07, 0x0F, 0x01, 0x01, 0x0F, 0x08, 0x08, 0x0F, 0x00}, // 0x32, 2
    {0x07, 0x0F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x0F, 0x00}, // 0x33, 3
    {0x07, 0x09, 0x09, 0x09, 0x0F, 0x01, 0x01, 0x01, 0x00}, // 0x34, 4
    {0x07, 0x0F, 0x08, 0x08, 0x0F, 0x01, 0x01, 0x0F, 0x00}, // 0x35, 5
    {0x07, 0x0F, 0x08, 0x08, 0x0F, 0x09, 0x09, 0x0F, 0x00}, // 0x36, 6
    {0x07, 0x0F, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00}, // 0x37, 7
    {0x07, 0x0F, 0x09, 0x09, 0x0F, 0x09, 0x09, 0x0F, 0x00}, // 0x38, 8
    {0x07, 0x0F, 0x09, 0x09, 0x0F, 0x01, 0x01, 0x0F, 0x00}, // 0x39, 9
    {0x04, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00}, // 0x3a, :
    {0x07, 0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08, 0x00}, // 0x3b, ;
    {0x07, 0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02, 0x00}, // 0x3c, <
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x3d, =
    {0x07, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x00}, // 0x3e, >
    {0x07, 0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04, 0x00}, // 0x3f, ?
    {0x07, 0x0e, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0f, 0x00}, // 0x40, @
    {0x07, 0x04, 0x0a, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x00}, // 0x41, A
    {0x07, 0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e, 0x00}, // 0x42, B
    {0x07, 0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e, 0x00}, // 0x43, C
    {0x07, 0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E, 0x00}, // 0x44, D
    {0x07, 0x1f, 0x10, 0x10, 0x1c, 0x10, 0x10, 0x1f, 0x00}, // 0x45, E
    {0x07, 0x1f, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10, 0x00}, // 0x46, F
    {0x07, 0x0e, 0x11, 0x10, 0x10, 0x13, 0x11, 0x0f, 0x00}, // 0x37, G
    {0x07, 0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x00}, // 0x48, H
    {0x07, 0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e, 0x00}, // 0x49, I
    {0x07, 0x1f, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c, 0x00}, // 0x4a, J
    {0x07, 0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11, 0x00}, // 0x4b, K
    {0x07, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00}, // 0x4c, L
    {0x07, 0x11, 0x1b, 0x15, 0x11, 0x11, 0x11, 0x11, 0x00}, // 0x4d, M
    {0x07, 0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x00}, // 0x4e, N
    {0x07, 0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00}, // 0x4f, O
    {0x07, 0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10, 0x00}, // 0x50, P
    {0x07, 0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d, 0x00}, // 0x51, Q
    {0x07, 0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11, 0x00}, // 0x52, R
    {0x07, 0x0e, 0x11, 0x10, 0x0e, 0x01, 0x11, 0x0e, 0x00}, // 0x53, S
    {0x07, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00}, // 0x54, T
    {0x07, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00}, // 0x55, U
    {0x07, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04, 0x00}, // 0x56, V
    {0x07, 0x11, 0x11, 0x11, 0x15, 0x15, 0x1b, 0x11, 0x00}, // 0x57, W
    {0x07, 0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11, 0x00}, // 0x58, X
    {0x07, 0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04, 0x00}, // 0x59, Y
    {0x07, 0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f, 0x00}, // 0x5a, Z
    {0x07, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x00}, // 0x5b, [
    {0x07, 0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01, 0x00}, // 0x5c, '\'
    {0x07, 0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e, 0x00}, // 0x5d, ]
    {0x07, 0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x5e, ^
    {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00}, // 0x5f, _
    {0x07, 0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x60, `
    {0x07, 0x00, 0x0e, 0x01, 0x0d, 0x13, 0x13, 0x0d, 0x00}, // 0x61, a
    {0x07, 0x10, 0x10, 0x10, 0x1c, 0x12, 0x12, 0x1c, 0x00}, // 0x62, b
    {0x07, 0x00, 0x00, 0x0E, 0x10, 0x10, 0x10, 0x0E, 0x00}, // 0x63, c
    {0x07, 0x01, 0x01, 0x01, 0x07, 0x09, 0x09, 0x07, 0x00}, // 0x64, d
    {0x07, 0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0f, 0x00}, // 0x65, e
    {0x07, 0x06, 0x09, 0x08, 0x1c, 0x08, 0x08, 0x08, 0x00}, // 0x66, f
    {0x07, 0x00, 0x0e, 0x11, 0x13, 0x0d, 0x01, 0x01, 0x0e}, // 0x67, g
    {0x07, 0x10, 0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x00}, // 0x68, h
    {0x05, 0x00, 0x02, 0x00, 0x06, 0x02, 0x02, 0x07, 0x00}, // 0x69, i
    {0x07, 0x00, 0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0c}, // 0x6a, j
    {0x07, 0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12, 0x00}, // 0x6b, k
    {0x05, 0x06, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00}, // 0x6c, l
    {0x07, 0x00, 0x00, 0x0a, 0x15, 0x15, 0x11, 0x11, 0x00}, // 0x6d, m
    {0x07, 0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11, 0x00}, // 0x6e, n
    {0x07, 0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e, 0x00}, // 0x6f, o
    {0x07, 0x00, 0x00, 0x1c, 0x12, 0x12, 0x1c, 0x10, 0x10}, // 0x70, p
    {0x07, 0x00, 0x00, 0x07, 0x09, 0x09, 0x07, 0x01, 0x01}, // 0x71, q
    {0x07, 0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10, 0x00}, // 0x72, r
    {0x07, 0x00, 0x00, 0x0f, 0x10, 0x0e, 0x01, 0x1e, 0x00}, // 0x73, s
    {0x07, 0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06, 0x00}, // 0x74, t
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d, 0x00}, // 0x75, u
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x11, 0x0a, 0x04, 0x00}, // 0x76, v
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a, 0x00}, // 0x77, w
    {0x07, 0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00}, // 0x78, x
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x0f, 0x01, 0x11, 0x0e}, // 0x79, y
    {0x07, 0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f, 0x00}, // 0x7a, z
    {0x07, 0x06, 0x08, 0x08, 0x10, 0x08, 0x08, 0x06, 0x00}, // 0x7b, {
    {0x07, 0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00}, // 0x7c, |
    {0x07, 0x0c, 0x02, 0x02, 0x01, 0x02, 0x02, 0x0c, 0x00}, // 0x7d, }
    {0x07, 0x08, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x7e, ~
    {0x07, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00}  // 0x7f, DEL
};

// Zeichensatz 5x8 in einer 8x8 Matrix, 0,0 ist rechts oben
unsigned short const font2[96][9] = {
    {0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00},                                     // 0x20, Space
    {0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04, 0x00}, // 0x21, !
    {0x07, 0x09, 0x09, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x22, "
    {0x07, 0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a, 0x00}, // 0x23, #
    {0x07, 0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04, 0x00}, // 0x24, $
    {0x07, 0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13, 0x00}, // 0x25, %
    {0x07, 0x04, 0x0a, 0x0a, 0x0a, 0x15, 0x12, 0x0d, 0x00}, // 0x26, &
    {0x07, 0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x27, '
    {0x07, 0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02, 0x00}, // 0x28, (
    {0x07, 0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00}, // 0x29, )
    {0x07, 0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04, 0x00}, // 0x2a, *
    {0x07, 0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00, 0x00}, // 0x2b, +
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02}, // 0x2c, ,
    {0x07, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00}, // 0x2d, -
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00}, // 0x2e, .
    {0x07, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10, 0x00}, // 0x2f, /
    {0x07, 0x00, 0x00, 0x07, 0x05, 0x05, 0x05, 0x07, 0x00}, // 0x30, 0
    {0x07, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00}, // 0x31, 1
    {0x07, 0x00, 0x00, 0x07, 0x01, 0x07, 0x04, 0x07, 0x00}, // 0x32, 2
    {0x07, 0x00, 0x00, 0x07, 0x01, 0x07, 0x01, 0x07, 0x00}, // 0x33, 3
    {0x07, 0x00, 0x00, 0x05, 0x05, 0x07, 0x01, 0x01, 0x00}, // 0x34, 4
    {0x07, 0x00, 0x00, 0x07, 0x04, 0x07, 0x01, 0x07, 0x00}, // 0x35, 5
    {0x07, 0x00, 0x00, 0x07, 0x04, 0x07, 0x05, 0x07, 0x00}, // 0x36, 6
    {0x07, 0x00, 0x00, 0x07, 0x01, 0x01, 0x01, 0x01, 0x00}, // 0x37, 7
    {0x07, 0x00, 0x00, 0x07, 0x05, 0x07, 0x05, 0x07, 0x00}, // 0x38, 8
    {0x07, 0x00, 0x00, 0x07, 0x05, 0x07, 0x01, 0x07, 0x00}, // 0x39, 9
    {0x04, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x00}, // 0x3a, :
    {0x07, 0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08, 0x00}, // 0x3b, ;
    {0x07, 0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02, 0x00}, // 0x3c, <
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x3d, =
    {0x07, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x00}, // 0x3e, >
    {0x07, 0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04, 0x00}, // 0x3f, ?
    {0x07, 0x0e, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0f, 0x00}, // 0x40, @
    {0x07, 0x04, 0x0a, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x00}, // 0x41, A
    {0x07, 0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e, 0x00}, // 0x42, B
    {0x07, 0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e, 0x00}, // 0x43, C
    {0x07, 0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E, 0x00}, // 0x44, D
    {0x07, 0x1f, 0x10, 0x10, 0x1c, 0x10, 0x10, 0x1f, 0x00}, // 0x45, E
    {0x07, 0x1f, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10, 0x00}, // 0x46, F
    {0x07, 0x0e, 0x11, 0x10, 0x10, 0x13, 0x11, 0x0f, 0x00}, // 0x37, G
    {0x07, 0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x00}, // 0x48, H
    {0x07, 0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e, 0x00}, // 0x49, I
    {0x07, 0x1f, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c, 0x00}, // 0x4a, J
    {0x07, 0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11, 0x00}, // 0x4b, K
    {0x07, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00}, // 0x4c, L
    {0x07, 0x11, 0x1b, 0x15, 0x11, 0x11, 0x11, 0x11, 0x00}, // 0x4d, M
    {0x07, 0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x00}, // 0x4e, N
    {0x07, 0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00}, // 0x4f, O
    {0x07, 0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10, 0x00}, // 0x50, P
    {0x07, 0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d, 0x00}, // 0x51, Q
    {0x07, 0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11, 0x00}, // 0x52, R
    {0x07, 0x0e, 0x11, 0x10, 0x0e, 0x01, 0x11, 0x0e, 0x00}, // 0x53, S
    {0x07, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00}, // 0x54, T
    {0x07, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00}, // 0x55, U
    {0x07, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04, 0x00}, // 0x56, V
    {0x07, 0x11, 0x11, 0x11, 0x15, 0x15, 0x1b, 0x11, 0x00}, // 0x57, W
    {0x07, 0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11, 0x00}, // 0x58, X
    {0x07, 0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04, 0x00}, // 0x59, Y
    {0x07, 0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f, 0x00}, // 0x5a, Z
    {0x07, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x00}, // 0x5b, [
    {0x07, 0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01, 0x00}, // 0x5c, '\'
    {0x07, 0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e, 0x00}, // 0x5d, ]
    {0x07, 0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x5e, ^
    {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00}, // 0x5f, _
    {0x07, 0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x60, `
    {0x07, 0x00, 0x0e, 0x01, 0x0d, 0x13, 0x13, 0x0d, 0x00}, // 0x61, a
    {0x07, 0x10, 0x10, 0x10, 0x1c, 0x12, 0x12, 0x1c, 0x00}, // 0x62, b
    {0x07, 0x00, 0x00, 0x0E, 0x10, 0x10, 0x10, 0x0E, 0x00}, // 0x63, c
    {0x07, 0x01, 0x01, 0x01, 0x07, 0x09, 0x09, 0x07, 0x00}, // 0x64, d
    {0x07, 0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0f, 0x00}, // 0x65, e
    {0x07, 0x06, 0x09, 0x08, 0x1c, 0x08, 0x08, 0x08, 0x00}, // 0x66, f
    {0x07, 0x00, 0x0e, 0x11, 0x13, 0x0d, 0x01, 0x01, 0x0e}, // 0x67, g
    {0x07, 0x10, 0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x00}, // 0x68, h
    {0x05, 0x00, 0x02, 0x00, 0x06, 0x02, 0x02, 0x07, 0x00}, // 0x69, i
    {0x07, 0x00, 0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0c}, // 0x6a, j
    {0x07, 0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12, 0x00}, // 0x6b, k
    {0x05, 0x06, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00}, // 0x6c, l
    {0x07, 0x00, 0x00, 0x0a, 0x15, 0x15, 0x11, 0x11, 0x00}, // 0x6d, m
    {0x07, 0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11, 0x00}, // 0x6e, n
    {0x07, 0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e, 0x00}, // 0x6f, o
    {0x07, 0x00, 0x00, 0x1c, 0x12, 0x12, 0x1c, 0x10, 0x10}, // 0x70, p
    {0x07, 0x00, 0x00, 0x07, 0x09, 0x09, 0x07, 0x01, 0x01}, // 0x71, q
    {0x07, 0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10, 0x00}, // 0x72, r
    {0x07, 0x00, 0x00, 0x0f, 0x10, 0x0e, 0x01, 0x1e, 0x00}, // 0x73, s
    {0x07, 0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06, 0x00}, // 0x74, t
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d, 0x00}, // 0x75, u
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x11, 0x0a, 0x04, 0x00}, // 0x76, v
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a, 0x00}, // 0x77, w
    {0x07, 0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00}, // 0x78, x
    {0x07, 0x00, 0x00, 0x11, 0x11, 0x0f, 0x01, 0x11, 0x0e}, // 0x79, y
    {0x07, 0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f, 0x00}, // 0x7a, z
    {0x07, 0x06, 0x08, 0x08, 0x10, 0x08, 0x08, 0x06, 0x00}, // 0x7b, {
    {0x07, 0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00}, // 0x7c, |
    {0x07, 0x0c, 0x02, 0x02, 0x01, 0x02, 0x02, 0x0c, 0x00}, // 0x7d, }
    {0x07, 0x08, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x7e, ~
    {0x07, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00}  // 0x7f, DEL
};
//**************************************************************************************************
bool autoConfig()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass); // 默认连接保存的WIFI

    for (int i = 0; i < 15; i++)
    {
        char2Arr('W', 28 + 32, 0);
        char2Arr('i', 22 + 32, 0);
        char2Arr('-', 18 + 32, 0);
        char2Arr('F', 12 + 32, 0);
        char2Arr('i', 6 + 32, 0);

        char2Arr('c', 28, 0);
        char2Arr('o', 22, 0);
        char2Arr('n', 16, 0);
        char2Arr('n', 10, 0);

        refresh_display();

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("AutoConfig Success");
            Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
            Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
            clear_Display();
            char2Arr('O', 25, 0);
            char2Arr('K', 19, 0);
            char2Arr('!', 12, 0);
            char2Arr('!', 6, 0);
            refresh_display();

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
            Serial.print("AutoConfig Waiting......");
            Serial.println(WiFi.status());
            delay(1000);
        }
    }
    clear_Display();
    char2Arr('E', 25, 0);
    char2Arr('r', 19, 0);
    char2Arr('r', 12, 0);
    char2Arr('!', 6, 0);
    refresh_display();
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
            char2Arr('O', 25, 0);
            char2Arr('K', 19, 0);
            char2Arr('!', 12, 0);
            char2Arr('!', 6, 0);
            refresh_display();
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
        clear_Display();
        char2Arr('S', 29, 0);
        char2Arr('-', 23, -1);
        char2Arr('c', 17, 0);
        char2Arr('o', 12, 0);
        char2Arr('n', 6, 0);
        refresh_display();
        delay(1000);
    }
    if (i > 18)
    {
        clear_Display();
        char2Arr('R', 25, 0);
        char2Arr('T', 19, 0);
        char2Arr('C', 12, 0);
        char2Arr('!', 6, 0);
        refresh_display();
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
void rtc_init(unsigned char sda, unsigned char scl)
{
    Wire.begin(sda, scl);
    rtc_Write(controlREG, 0x00);
}
//**************************************************************************************************
// BCD Code
//**************************************************************************************************
unsigned char dec2bcd(unsigned char x)
{ //value 0...99
    unsigned char z, e, r;
    e = x % 10;
    z = x / 10;
    z = z << 4;
    r = e | z;
    return (r);
}
unsigned char bcd2dec(unsigned char x)
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
    return (bcd2dec(rtc_Read(secondREG)));
}
unsigned char rtc_minute()
{
    return (bcd2dec(rtc_Read(minuteREG)));
}
unsigned char rtc_stunde()
{
    return (bcd2dec(rtc_Read(hourREG)));
}
unsigned char rtc_wochentag()
{
    return (bcd2dec(rtc_Read(WTREG)));
}
unsigned char rtc_tag()
{
    return (bcd2dec(rtc_Read(dateREG)));
}
unsigned char rtc_monat()
{
    return (bcd2dec(rtc_Read(monthREG)));
}
unsigned char rtc_jahr()
{
    return (bcd2dec(rtc_Read(yearREG)));
}
void rtc_second(unsigned char sek)
{
    rtc_Write(secondREG, (dec2bcd(sek)));
}
void rtc_minute(unsigned char min)
{
    rtc_Write(minuteREG, (dec2bcd(min)));
}
void rtc_stunde(unsigned char std)
{
    rtc_Write(hourREG, (dec2bcd(std)));
}
void rtc_wochentag(unsigned char wt)
{
    rtc_Write(WTREG, (dec2bcd(wt)));
}
void rtc_tag(unsigned char tag)
{
    rtc_Write(dateREG, (dec2bcd(tag)));
}
void rtc_monat(unsigned char mon)
{
    rtc_Write(monthREG, (dec2bcd(mon)));
}
void rtc_jahr(unsigned char jahr)
{
    rtc_Write(yearREG, (dec2bcd(jahr)));
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
    lowByte = rtc_Read(tempLSBREG);
    highByte = rtc_Read(tempMSBREG);
    lowByte >>= 6;
    lowByte &= 0x03;
    t = ((float)lowByte);
    t *= 0.25;
    t += highByte;
    return (t); // return temp value
}
//**************************************************************************************************
void rtc2mez()
{

    unsigned short Jahr, Tag, Monat, WoTag, Stunde, Minute, Sekunde;

    mTime.jahr = rtc_jahr(); //年
    if (mTime.jahr > 99)
        mTime.jahr = 0;
    mTime.month = rtc_monat(); //月
    if (mTime.month > 12)
        mTime.month = 0;
    mTime.day = rtc_tag(); //天
    if (mTime.day > 31)
        mTime.day = 0;
    mTime.week = rtc_wochentag(); //So=0, Mo=1, Di=2 ...
    if (mTime.week == 7)
        mTime.week = 0;
    mTime.hour = rtc_stunde(); //小时
    if (mTime.hour > 23)
        mTime.hour = 0;
    mTime.minute = rtc_minute(); //分钟
    if (mTime.minute > 59)
        mTime.minute = 0;
    mTime.second = rtc_second(); //秒
    if (mTime.second > 59)
        mTime.second = 0;

    Serial.print("time: ");
    Serial.print(mTime.hour);
    Serial.print(mTime.minute);
    Serial.println(mTime.second);
}

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
void max7219_init() //all MAX7219 init
{
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
//**************************************************************************************************
void clear_Display() //clear all
{
    unsigned short i, j;
    for (i = 0; i < 8; i++) //8 rows
    {
        digitalWrite(CS, LOW);
        delayMicroseconds(1);
        for (j = anzMAX; j > 0; j--)
        {
            LEDarr[j - 1][i] = 0; //LEDarr clear
            SPI.write(i + 1);     //current row
            SPI.write(LEDarr[j - 1][i]);
        }
        digitalWrite(CS, HIGH);
    }
}
//*********************************************************************************************************
void rotate_90() // for Generic displays
{
    for (uint8_t k = anzMAX; k > 0; k--)
    {

        uint8_t i, j, m, imask, jmask;
        uint8_t tmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        for (i = 0, imask = 0x01; i < 8; i++, imask <<= 1)
        {
            for (j = 0, jmask = 0x01; j < 8; j++, jmask <<= 1)
            {
                if (LEDarr[k - 1][i] & jmask)
                {
                    tmp[j] |= imask;
                }
            }
        }
        for (m = 0; m < 8; m++)
        {
            LEDarr[k - 1][m] = tmp[m];
        }
    }
}
//**************************************************************************************************
void refresh_display() //take info into LEDarr
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
            SPI.write(LEDarr[j - 1][7 - i]);
#else
            SPI.write(LEDarr[j - 1][i]);
#endif

#ifdef REVERSE_HORIZONTAL
            SPI.setBitOrder(MSBFIRST); // reset bitorder
#endif
        }
        digitalWrite(CS, HIGH);
    }
}
//**************************************************************************************************
void char2Arr(unsigned short ch, int PosX, short PosY)
{                                      //characters into arr
    int i, j, k, l, m, o1, o2, o3, o4; //in LEDarr
    PosX++;
    k = ch - 32;              //ASCII position in font
    if ((k >= 0) && (k < 96)) //character found in font?
    {
        o4 = font1[k][0]; //character width
        o3 = 1 << (o4 - 2);
        for (i = 0; i < o4; i++)
        {
            if (((PosX - i <= maxPosX) && (PosX - i >= 0)) && ((PosY > -8) && (PosY < 8))) //within matrix?
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
                            LEDarr[o2][j - PosY] = LEDarr[o2][j - PosY] | (o1); //set point
                        else
                            LEDarr[o2][j - PosY] = LEDarr[o2][j - PosY] & (~o1); //clear point
                    }
                }
            }
        }
    }
}

void char22Arr(unsigned short ch, int PosX, short PosY)
{                                      //characters into arr
    int i, j, k, l, m, o1, o2, o3, o4; //in LEDarr
    PosX++;
    k = ch - 32;              //ASCII position in font
    if ((k >= 0) && (k < 96)) //character found in font?
    {
        o4 = font2[k][0]; //character width
        o3 = 1 << (o4 - 2);
        for (i = 0; i < o4; i++)
        {
            if (((PosX - i <= maxPosX) && (PosX - i >= 0)) && ((PosY > -8) && (PosY < 8))) //within matrix?
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
                            LEDarr[o2][j - PosY] = LEDarr[o2][j - PosY] | (o1); //set point
                        else
                            LEDarr[o2][j - PosY] = LEDarr[o2][j - PosY] & (~o1); //clear point
                    }
                }
            }
        }
    }
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

    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
    Serial.begin(115200);
    SPI.begin();
    helpArr_init();
    max7219_init();
    rtc_init(SDA, SCL);
    clear_Display();
    refresh_display();            //take info into LEDarr
    tckr.attach(0.05, timer50ms); // every 50 msec
    max7219_set_brightness(3);
    //////////////////////////////////
    if (!autoConfig())
    {
        smartConfig();
    }
    ///////////////////////////////////
    gtt = connectNTP();
    if (gtt != NULL)
        rtc_set(gtt);
    else
        Serial.println("no timepacket received");

    clear_Display();
}


//**************************************************************************************************
// The loop function is called in an endless loop
void loop()
{
    //Add your repeated code here
    struct DateTime mPerviousTime;
    struct DateTime mPresentTime;

    signed int x = 0; //x1,x2;
    signed int y = 0, y1 = 0, y2 = 0, y3 = 0;
    bool updown = false;
    bool f_scrollend_y = false;
    unsigned int f_scroll_x = false;

    //滚动模式
    unsigned char mVarietyType = 0;

    z_PosX = maxPosX;
    d_PosX = maxPosX;
    //  x=0; x1=0; x2=0;

    refresh_display();
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
        if (mTime.hour == 0 && mTime.minute == 20 && mTime.second == 0) //syncronisize RTC every day 00:20:00
        {
            clear_Display();
            delay(500);
            ESP.restart();
        }

        //修改逻辑200ms读取一次rtc时钟，判断秒数变化再做更新
        if (f_tckr1s == true) // flag 1sek
        {
            rtc2mez();

            mVarietyType = 0;
            if(mTime.second != mPresentTime.second)
            {
                mPerviousTime.second = mPresentTime.second;
                mPresentTime.second = mTime.second;
                
                mVarietyType |= 1;
                if(mTime.second%10 == 0)
                    mVarietyType |= 0x2;
                
            }

            if(mTime.minute != mPresentTime.minute)
            {
                mPerviousTime.minute = mPresentTime.minute;
                mPresentTime.minute = mTime.minute;
                mVarietyType |= 0x4;
                if(mTime.minute%10 == 0)
                    mVarietyType |= 0x8;
            }

            
            if(mTime.hour != mPresentTime.hour)
            {
                mPerviousTime.hour = mPresentTime.hour;
                mPresentTime.hour = mTime.hour;
                mVarietyType |= 0x10;
                if(mTime.hour%10 == 0)
                    mVarietyType |= 0x20;
            }

            y = y2; //scroll updown
            f_tckr1s = false;

            // if (mTime.second == 45)
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
                if (z_PosX == maxPosX)
                {
                    f_scroll_x = false;
                    d_PosX = -8;
                }
            }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            char22Arr(48 + mTime.day % 10, z_PosX - 59, 0);
            char22Arr(48 + mTime.day / 10, z_PosX - 55, 0);
            char22Arr(48 + mTime.month % 10, z_PosX - 51, 0);
            char22Arr(48 + mTime.month / 10, z_PosX - 47, 0);
            char22Arr(48 + mTime.jahr % 10, z_PosX - 43, 0);
            char22Arr(48 + mTime.jahr / 10, z_PosX - 39, 0);
            char22Arr('0', z_PosX - 35, 0);
            char22Arr('2', z_PosX - 31, 0); //year

            // char2Arr(' ', d_PosX+5, 0);        //day of the week
            // char2Arr(WT_arr[MEZ.WT][0], d_PosX - 1, 0);        //day of the week
            // char2Arr(WT_arr[MEZ.WT][1], d_PosX - 7, 0);
            // char2Arr(WT_arr[MEZ.WT][2], d_PosX - 13, 0);
            // char2Arr(WT_arr[MEZ.WT][3], d_PosX - 19, 0);
            // char2Arr(48 + MEZ.tag2, d_PosX - 24, 0);           //day
            // char2Arr(48 + MEZ.tag1, d_PosX - 30, 0);
            // char2Arr(M_arr[MEZ.mon12 - 1][0], d_PosX - 39, 0); //month
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
                char22Arr(48 + mPresentTime.second%10, z_PosX - 27, y3);
                char22Arr(48 + mPerviousTime.second%10, z_PosX - 27, y + y1);
                if (y == 0)
                {
                    mVarietyType &= 0xFE;
                    f_scrollend_y = true;
                }
            }
            else
                char22Arr(48 + mPresentTime.second%10, z_PosX - 27, 0);

            if((mVarietyType&0x2) == 0x2) //秒十位变化
            {        
                char22Arr(48 + mPresentTime.second/10, z_PosX - 23, y3);
                char22Arr(48 + mPerviousTime.second/10, z_PosX - 23, y + y1);
                if (y == 0)
                {
                    mVarietyType &= 0xFD;
                }
            }
            else
            {
                char22Arr(48 + mPresentTime.second/10, z_PosX - 23, 0);
            }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    
            if((mVarietyType&0x4) == 0x4) //分个位变化
            {
                char2Arr(48 + mPresentTime.minute%10, z_PosX - 18, y);
                char2Arr(48 + mPerviousTime.minute%10, z_PosX - 18, y + y1);
                if (y == 0)
                    mVarietyType &= 0xFB;
            }
            else
                char2Arr(48 + mPresentTime.minute%10, z_PosX - 18, 0);

            if((mVarietyType&0x8) == 0x8) //分十位变化
            {
                char2Arr(48 + mPresentTime.minute/10, z_PosX - 13, y);
                char2Arr(48 + mPerviousTime.minute/10, z_PosX - 13, y + y1);
                if (y == 0)
                    mVarietyType &= 0xF7;
            }
            else
                char2Arr(48 + mPresentTime.minute/10, z_PosX - 13, 0);

            char2Arr(':', z_PosX - 10 + x, 0);

            if((mVarietyType&0x10) == 0x10) //时个位变化
            {
                char2Arr(48 + mPresentTime.hour%10, z_PosX - 4, y);
                char2Arr(48 + mPerviousTime.hour%10, z_PosX - 4, y + y1);
                if (y == 0)
                    mVarietyType &= 0xEF;
            }
            else
                char2Arr(48 + mPresentTime.hour%10, z_PosX - 4, 0);

            if((mVarietyType&0x20) == 0x20) //时十位变化
            {
                char2Arr(48 + mPresentTime.hour/10, z_PosX + 1, y);
                char2Arr(48 + mPerviousTime.hour/10, z_PosX + 1, y + y1);
                if (y == 0)
                    mVarietyType &= 0xDF;
            }
            else
                char2Arr(48 + mPresentTime.hour/10, z_PosX + 1, 0);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            refresh_display(); //alle 50ms
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
