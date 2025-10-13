/* *********************************************************************************
 * ESP QRcode
 * dependency library :
 *   Adafruit_GFX
 *   https://github.com/lewisxhe/GxEPD
 *
 * Example using TTGO T5 E-Ink display 
 * 
 ***********************************************************************************/
#define EINKDISPLAY
#include <GxEPD.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <qrcode.h>

#define SPI_MOSI 23
#define SPI_MISO -1
#define SPI_CLK 18

#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17

GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);
QRcode qrcode (&display);

void setup() {
     display.init();
     qrcode.init();
     // create qrcode
     qrcode.create("Hello world.");
}

void loop() {

}