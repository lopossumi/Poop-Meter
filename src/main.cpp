#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>

// OLED
#define CLK 13
#define MOSI 11
#define CS1 10
#define RES1 9
#define DC 8

//Ultrasound
#define RX 7
#define TX 6

U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R3, CLK, MOSI, CS1, DC, RES1);
SoftwareSerial swSerial(RX, TX); // RX, TX

unsigned char data[4] = {};
float distance;

float max_distance = 500.0; //mm
float min_distance = 30.0;  //mm

void setup()
{
    u8g2.begin();
    u8g2.setFont(u8g2_font_courB14_tr);
    swSerial.begin(9600);
}

void drawTank(int percentage)
{
    const int frame_top = 35; //px
    const int margin = 3;     //px
    int x_position;           //px

    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    if (percentage < 10) x_position = 22;
    else if (percentage < 100) x_position = 16;
    else x_position = 10;

    int tankHeight_px = u8g2.getHeight() - (frame_top + 2 * margin);
    int liquidHeight_px = tankHeight_px * (percentage / 100.0);

    u8g2.clearBuffer();
    u8g2.setCursor(x_position, 21);
    u8g2.print(percentage);
    u8g2.print("%");
    u8g2.drawFrame(0, frame_top, u8g2.getWidth(), u8g2.getHeight() - frame_top);
    u8g2.drawBox(margin, (frame_top + margin) + (tankHeight_px - liquidHeight_px), u8g2.getWidth() - margin * 2, liquidHeight_px);
    u8g2.sendBuffer();
}

void loop()
{
    // Read A02YYUW ultrasonic distance sensor output
    do
    {
        for (int i = 0; i < 4; i++)
        {
            data[i] = swSerial.read();
        }
    } while (swSerial.read() == 0xff);
    swSerial.flush();

    if (data[0] == 0xff)
    {
        int sum;
        sum = (data[0] + data[1] + data[2]) & 0x00FF;
        if (sum == data[3])
        {
            distance = (data[1] << 8) + data[2];
            float tank_percentage = 100.0 - (distance - min_distance) / max_distance * 100.0;
            drawTank((int)tank_percentage);
        }
    }
    delay(50);
}