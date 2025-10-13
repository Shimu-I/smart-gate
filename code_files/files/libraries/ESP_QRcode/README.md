# ESP QRcode

Just copy the ESP QRcode folder to your Arduino 'libraries' folder. I have included an example sketch.

The code is based on qrdquino by tz1 : https://github.com/tz1/qrduino

This code is a fork of https://github.com/anunpanya/ESP8266_QRcode

## Install dependency for ESP8266 QRcode
Open Library Manager (menu Sketch > Include Library > Manage Libraries…) then install the following libraries
For OLED:
- ESP8266 Oled Driver for SSD1306 display by Daniel Eichborn, Fabrice Weinberg 

For TFT:
- Adafruit GFX 
- Adafruit ST7735 (or ST7789)

For eink display:
- Adafruit GFX 
- https://github.com/lewisxhe/GxEPD

Depending on the display you want to support you have to uncomment the corresponding
define in qrcode.h (or keep all commented an define it in your own code)

## Run examples from ESP8266 QRcode
- Open Arduino IDE, try examples of ESP QRcode library

## Display example
![qrcode](src/img/qrcode.JPG?raw=true)

## TFT example
![qrcode](src/img/tftqr.jpg?raw=true)

## EINK Example
![qrcode](src/img/einkqr.jpg?raw=true)
