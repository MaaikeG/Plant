#include "ScreenCarousel.h"


// void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
    
// }

// void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//      }

ScreenCarousel::ScreenCarousel(SSD1306* _oled, SensorsController _sensorsController):
    sensorsController(_sensorsController),
    ui(_oled)
{
    oled = _oled;
}

void ScreenCarousel::init()  {
    FrameCallback frame1 = static_cast<FrameCallback>([=](OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y){ drawFrame1(display, state, x, y);});
    //this->drawFrame1(display, state, x, y);
    
    FrameCallback frames[] = { frame1};
    int frameCount = 2;

    ui.setFrameAnimation(SLIDE_LEFT);
    ui.setFrames(frames, frameCount);
    ui.init();
    oled->flipScreenVertically();
}

void ScreenCarousel::drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
    char res[24];
    char buff[5];
    dtostrf(sensorsController.getTemperature(), 2, 1, buff);
    sprintf(res, "Temperature: %s C", buff);
    display->drawString(x, y, res);

    dtostrf(sensorsController.getHumidity(), 2, 1, buff);
    sprintf(res, "Humidity: %s%%", buff);
    display->drawString(x, y + 10, res);
}

void ScreenCarousel::drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    char res[24];
    char buff[5];
    dtostrf(sensorsController.getPressure(), 4, 1, buff);
    sprintf(res, "Pressure: %s hPa", buff);
    display->drawString(x, y, res);

    dtostrf(sensorsController.getSoilMoisture(), 2, 1, buff);
    sprintf(res, "Soil moisture: %s%%", buff);
    display->drawString(x, y+10, res);

    dtostrf(sensorsController.getLightIntensity(), 2, 1, buff);
    sprintf(res, "Light: %s%%", buff);
    display->drawString(x, y + 20, res);
 }

 void ScreenCarousel::test(int i)
 {
 }