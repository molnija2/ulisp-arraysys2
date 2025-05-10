#ifndef ___ESP32_2432S028R
#define ___ESP32_2432S028R


#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
// Please edit "User_Setup.h" file in the  /home/sap/Arduino/libraries/TFT_eSPI folder
#include <SPI.h>
#define COLOR_WHITE TFT_WHITE
#define COLOR_BLACK TFT_BLACK
#define COLOR_GREEN 0x07e0

// TFT Screen pixel resolution in landscape orientation, change these to suit your display
// Defined in landscape orientation !
#define HRES 320
#define VRES 240
#define TFT_BACKLITE 21
TFT_eSPI tft = TFT_eSPI(320,240);  // Invoke library, pins defined in User_Setup.h

#if defined(touchscreen_support)
#include <TFT_Touch.h> // Touchscreen driver chip

#define XPT2046_IRQ			36
#define XPT2046_MOSI		32
#define XPT2046_MISO		39
#define XPT2046_CLK			25
#define XPT2046_CS			33
TFT_Touch tft_touch = TFT_Touch(XPT2046_CS, XPT2046_CLK, XPT2046_MOSI, XPT2046_MISO) ;
#endif   // TOUCHSCREEN


#endif   // ___ESP32_2432S028R
