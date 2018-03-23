#ifndef _ScreenCarousel_H
#define _ScreenCarousel_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SensorsController.h"
#include <SSD1306.h>
#include <Wire.h>
#include "OLEDDisplayUi.h"


class ScreenCarousel
{
 private:
	 SensorsController* sensorsController;
     SSD1306* oled;
     OLEDDisplayUi ui;
 
 public:
    ScreenCarousel(SSD1306* _oled, SensorsController* _sensorsController);
    int update();
    void begin(FrameCallback frames[], uint8_t frameCount);
    void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y);
    void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
};

#endif

