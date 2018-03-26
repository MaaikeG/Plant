#ifndef _ScreenCarousel_H
#define _ScreenCarousel_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SensorsController.h"
#include "WateringController.h"
#include <SSD1306.h>
#include <Wire.h>
#include "OLEDDisplayUi.h"
#include <TimeLib.h>


class ScreenCarousel
{
 private:
	 SensorsController* sensorsController;
     WateringController* wateringController;
     SSD1306* oled;
     OLEDDisplayUi ui;
 
 public:
    ScreenCarousel(SSD1306* _oled, SensorsController* _sensorsController, WateringController* _wateringController);
    int update();
    void begin(FrameCallback frames[], uint8_t frameCount);
    void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
};

#endif

