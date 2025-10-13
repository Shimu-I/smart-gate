/* ESP_QRcode. multidisplay version
 * Depending the libraries you have installed/you plan to use uncomment the 
 * corresponding defines previous to compilation
 */

// Uncomment the supported display for your project
//#define OLEDDISPLAY
//#define TFTDISPLAY
//#define EINKDISPLAY

#ifdef OLEDDISPLAY
#include "OLEDDisplay.h"
#endif
#if defined(TFTDISPLAY) || defined(EINKDISPLAY)
#include <Adafruit_GFX.h>   
#endif
#ifdef TFTDISPLAY
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#endif
#ifdef EINKDISPLAY
#include <GxEPD.h>
#include <GxGDE0213B72B/GxGDE0213B72B.h>
#endif


#define OLED_MODEL 255
#define EINK_MODEL 128

class QRcode
{
	private:
	#ifdef OLEDDISPLAY
		OLEDDisplay *display;
	#endif
	#ifdef TFTDISPLAY
		Adafruit_ST77xx *tft;
	#endif
	#ifdef EINKDISPLAY
		GxGDE0213B72B *eink;
	#endif
		uint8_t model;
		void render(int x, int y, int color);

	public:
	#ifdef OLEDDISPLAY
		QRcode(OLEDDisplay *display);
	#endif
	#ifdef TFTDISPLAY
		QRcode(Adafruit_ST7735 *display, uint8_t model);
		QRcode(Adafruit_ST7789 *display, uint8_t model);
	#endif
	#ifdef EINKDISPLAY
		QRcode(GxGDE0213B72B *display);
	#endif

		void init(uint16_t width, uint16_t height);
		void init();
		void debug();
		void screenwhite();
		void create(String message);
		
};
