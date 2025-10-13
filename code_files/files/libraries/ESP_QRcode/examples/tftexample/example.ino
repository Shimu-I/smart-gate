/* *********************************************************************************
 * ESP QRcode
 * dependency library :
 *   Adafruit_GFX
 *   Adafruit_ST7735
 *
 * Example using 1.44 ST7735 TFT display. 
 * ST7735 TFT SPI module connections
 *   TFT SCL(CLK) ---> NodeMCU pin D5 (GPIO14) ---> ESP32 pin 18 (GPIO18)
 *   TFT SDA(DIN) ---> NodeMCU pin D7 (GPIO13) ---> ESP32 pin 23 (GPIO23)
 * 
 ***********************************************************************************/
#define TFTDISPLAY
#include <Arduino.h>

#include <Adafruit_GFX.h>      
#include <Adafruit_ST7735.h>   
#include <SPI.h>
#include <qrcode.h>


#ifdef ARDUINO_ARCH_ESP32
#define TFT_RST   25    // TFT RST -> D25 (GPIO25) on ESP32
#define TFT_CS    5     // TFT CS  -> D5 (GPIO5)
#define TFT_DC    26    // TFT A0  -> D26 (GPIO26)
#else
#define TFT_RST   D6     // TFT RS  -> D6 on Wemos D1
#define TFT_CS    D8     // TFT CS  -> D8 (GPIO8)
#define TFT_DC    D4     // TFT A0  -> D4 (GPIO4)
#endif

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
QRcode qrcode (&tft,INITR_144GREENTAB);

void setup() {
     tft.initR(INITR_144GREENTAB);
     qrcode.init();
     // create qrcode
     qrcode.create("Hello world.");
}

void loop() {

}