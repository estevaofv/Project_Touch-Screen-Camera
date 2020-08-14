#include <Wire.h>
#include "SPI.h"
#include <Adafruit_GFX.h>
#include "ILI9488.h"

//SPI
#define SPI_MOSI 13
#define SPI_MISO 12
#define SPI_SCK 14

//TFT
#define TFT_CS 15
#define TFT_DC 33
#define TFT_LED -1 //1//-1
#define TFT_RST -1 //3//-1

//SPI control
#define SPI_ON_TFT digitalWrite(TFT_CS, LOW)
#define SPI_OFF_TFT digitalWrite(TFT_CS, HIGH)

#define ESP32_SDA 26
#define ESP32_SCL 27

#define TOUCH_I2C_ADD 0x38

#define TOUCH_REG_XL 0x04
#define TOUCH_REG_XH 0x03
#define TOUCH_REG_YL 0x06
#define TOUCH_REG_YH 0x05

ILI9488 tft = ILI9488(TFT_CS, TFT_DC, TFT_RST);

int readTouchReg(int reg)
{
    int data = 0;
    Wire.beginTransmission(TOUCH_I2C_ADD);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(TOUCH_I2C_ADD, 1);
    if (Wire.available())
    {
        data = Wire.read();
    }
    return data;
}

int getTouchPointX()
{
    int XL = 0;
    int XH = 0;

    XH = readTouchReg(TOUCH_REG_XH);
    XL = readTouchReg(TOUCH_REG_XL);

    return ((XH & 0x0F) << 8) | XL;
}

int getTouchPointY()
{
    int YL = 0;
    int YH = 0;

    YH = readTouchReg(TOUCH_REG_YH);
    YL = readTouchReg(TOUCH_REG_YL);

    return ((YH & 0x0F) << 8) | YL;
}

void setup()
{

    Serial.begin(115200);
    while (!Serial)
        ; // Leonardo: wait for serial monitor
    Serial.println("\n ft6236 test");

    Wire.begin(ESP32_SDA, ESP32_SCL);
    byte error, address;

    Wire.beginTransmission(TOUCH_I2C_ADD);
    error = Wire.endTransmission();

    if (error == 0)
    {
        Serial.print("I2C device found at address 0x");
        Serial.print(TOUCH_I2C_ADD, HEX);
        Serial.println("  !");
    }
    else if (error == 4)
    {
        Serial.print("Unknown error at address 0x");
        Serial.println(TOUCH_I2C_ADD, HEX);
    }

    pinMode(TFT_CS, OUTPUT);
    SPI_OFF_TFT;
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    //TFT(SPI) init
    SPI_ON_TFT;
    tft.begin();
    tft.fillScreen(ILI9488_BLUE);
    SPI_OFF_TFT;
}

void loop()
{
    int x = 0;
    int y = 0;

    x = getTouchPointX();
    y = getTouchPointY();
    Serial.println(x);
    Serial.println(y);

    Serial.println("...........");

    tft.fillRect(x, y, 5, 5, ILI9488_RED);

    //delay(100); // wait 5 seconds for next scan
}