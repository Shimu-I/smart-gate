#include <Arduino.h>
#include "qrcode.h"
#include "qrencode.h"

int offsetsX = 42;
int offsetsY = 10;
int screenwidth = 128;
int screenheight = 64;
bool QRDEBUG = false;
int multiply = 1;

#ifdef OLEDDISPLAY
QRcode::QRcode(OLEDDisplay *display){
	this->display = display;
  this->model = OLED_MODEL;
}
#endif

#ifdef TFTDISPLAY
QRcode::QRcode(Adafruit_ST7735 *tft, uint8_t model){
	this->tft = tft;
  this->model = model;
  offsetsX = 20;
  offsetsY = 20;
}

QRcode::QRcode(Adafruit_ST7789 *tft, uint8_t model){
	this->tft = tft;
  this->model = model;
  offsetsX = 20;
  offsetsY = 20;
}
#endif

#ifdef EINKDISPLAY
QRcode::QRcode(GxGDE0213B72B *eink){
	this->eink = eink;
  this->model = EINK_MODEL;
  offsetsX = 20;
  offsetsY = 20;
}
#endif

void QRcode::init(){
	if (this->model==OLED_MODEL){
    #ifdef OLEDDISPLAY
    display->init();
    display->flipScreenVertically();
    display->setColor(WHITE);
    multiply = 1;
    #endif
  } else {
    if (this->model==EINK_MODEL) {
    #ifdef EINKDISPLAY
      eink->init();
      screenwidth = eink->width();
      screenheight = eink->height();
      eink->setRotation(1);
      eink->eraseDisplay();
      int min = screenwidth;
      if (screenheight<screenwidth)
        min = screenheight;
      multiply = min/WD;
      offsetsX = (screenwidth-(WD*multiply))/2;
      offsetsY = (screenheight-(WD*multiply))/2;
    #endif
    } else {
      #ifdef TFTDISPLAY
      ((Adafruit_ST7735 *)tft)->initR(model);
      screenwidth = tft->width();
      screenheight = tft->height(); 
      //tft->setRotation(1);
      tft->fillScreen(ST77XX_WHITE);
      int min = screenwidth;
      if (screenheight<screenwidth)
        min = screenheight;
      multiply = min/WD;
      offsetsX = (screenwidth-(WD*multiply))/2;
      offsetsY = (screenheight-(WD*multiply))/2;
      #endif
    }
  }
}

void QRcode::init(uint16_t width, uint16_t height){
	if (this->model==OLED_MODEL){
    #ifdef OLEDDISPLAY
    display->init();
    display->flipScreenVertically();
    display->setColor(WHITE);
    #endif
  } else 
    if (this->model==EINK_MODEL) {
      #ifdef EINKDISPLAY
      eink->init();
      screenwidth = eink->width();
      screenheight = eink->height();
      eink->setRotation(1);
      eink->eraseDisplay();
      int min = screenwidth;
      if (screenheight<screenwidth)
        min = screenheight;
      multiply = min/WD;
      offsetsX = (screenwidth-(WD*multiply))/2;
      offsetsY = (screenheight-(WD*multiply))/2;
      #endif
    } else {
      #ifdef TFTDISPLAY
      ((Adafruit_ST7789 *)tft)->init(width,height);
      screenwidth = tft->width();
      screenheight = tft->height(); 
      //tft->setRotation(1);
      tft->fillScreen(ST77XX_WHITE);
      int min = screenwidth;
      if (screenheight<screenwidth)
        min = screenheight;
      multiply = WD/min;
      offsetsX = (screenwidth-(WD*multiply))/2;
      offsetsY = (screenheight-(WD*multiply))/2;
    #endif
  }
}

void QRcode::debug(){
	QRDEBUG = true;
}

void QRcode::render(int x, int y, int color){
  
  if (model != OLED_MODEL)
    multiply = 2;
  x=(x*multiply)+offsetsX;
  y=(y*multiply)+offsetsY;
  if(color==1) {
    if (model==OLED_MODEL) {
      #ifdef OLEDDISPLAY
	    display->setColor(BLACK);
      display->setPixel(x, y);
      #endif
    } else 
      if (this->model==EINK_MODEL) {
        #ifdef EINKDISPLAY
        eink->drawPixel(x,y,GxEPD_BLACK);
        if (multiply>1) {
          eink->drawPixel(x+1,y,GxEPD_BLACK);
          eink->drawPixel(x+1,y+1,GxEPD_BLACK);
          eink->drawPixel(x,y+1,GxEPD_BLACK);
        }  
        #endif
      } else {
        #ifdef TFTDISPLAY
        tft->drawPixel(x,y,ST77XX_BLACK);
        if (multiply>1) {
          tft->drawPixel(x+1,y,ST77XX_BLACK);
          tft->drawPixel(x+1,y+1,ST77XX_BLACK);
          tft->drawPixel(x,y+1,ST77XX_BLACK);
        }
        #endif
    }
  }
  else {
    if (model==OLED_MODEL) {
      #ifdef OLEDDISPLAY
	    display->setColor(WHITE);
      display->setPixel(x, y);
      #endif
    } else 
    if (this->model==EINK_MODEL) {
      #ifdef EINKDISPLAY
        eink->drawPixel(x,y,GxEPD_WHITE);
        if (multiply>1) {
          eink->drawPixel(x+1,y,GxEPD_WHITE);
          eink->drawPixel(x+1,y+1,GxEPD_WHITE);
          eink->drawPixel(x,y+1,GxEPD_WHITE);
        }  
      #endif
      } else {
        #ifdef TFTDISPLAY
        tft->drawPixel(x,y,ST77XX_WHITE);
        if (multiply>1) {
          tft->drawPixel(x+1,y,ST77XX_WHITE);
          tft->drawPixel(x+1,y+1,ST77XX_WHITE);
          tft->drawPixel(x,y+1,ST77XX_WHITE);
        }
      #endif
    }
  }
}

void QRcode::screenwhite(){
  if (model==OLED_MODEL) {
    #ifdef OLEDDISPLAY
      display->clear();
      display->setColor(WHITE);
      display->fillRect(0, 0, screenwidth, screenheight);
      display->display();
    #endif
  } else 
    if (this->model==EINK_MODEL) {
      #ifdef EINKDISPLAY
      eink->fillScreen(GxEPD_WHITE);
      #endif
    } else {
      #ifdef TFTDISPLAY
      tft->fillScreen(ST77XX_WHITE);
      #endif
  }
}

void QRcode::create(String message) {

  // create QR code
  message.toCharArray((char *)strinbuf,260);
  qrencode();
  screenwhite();

  
  // print QR Code
  for (byte x = 0; x < WD; x+=2) {
    for (byte y = 0; y < WD; y++) {
      if ( QRBIT(x,y) &&  QRBIT((x+1),y)) {
        // black square on top of black square
        render(x, y, 1);
        render((x+1), y, 1);
      }
      if (!QRBIT(x,y) &&  QRBIT((x+1),y)) {
        // white square on top of black square
        render(x, y, 0);
        render((x+1), y, 1);
      }
      if ( QRBIT(x,y) && !QRBIT((x+1),y)) {
        // black square on top of white square
        render(x, y, 1);
        render((x+1), y, 0);
      }
      if (!QRBIT(x,y) && !QRBIT((x+1),y)) {
        // white square on top of white square
        render(x, y, 0);
        render((x+1), y, 0);
      }
    }
  }
  if (model==OLED_MODEL){
    #ifdef OLEDDISPLAY
    display->display();
    #endif
  }
  if (this->model==EINK_MODEL) {
    #ifdef EINKDISPLAY
      eink->update();
    #endif
  }
}
