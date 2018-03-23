#include "ScreenCarousel.h"

ScreenCarousel::ScreenCarousel(SSD1306* _oled, SensorsController* _sensorsController):
    ui(_oled)
{
    sensorsController = _sensorsController;
    oled = _oled;
}

void ScreenCarousel::begin(FrameCallback frames[], uint8_t frameCount){
    ui.setTargetFPS(30);
    ui.setIndicatorPosition(BOTTOM);
    ui.setFrameAnimation(SLIDE_LEFT);
    ui.setIndicatorDirection(LEFT_RIGHT);
    ui.setFrames(frames, frameCount);
    ui.init();
}

void ScreenCarousel::drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    char res[24];
    char buff[5];
    dtostrf(sensorsController->getTemperature(), 2, 1, buff);
    sprintf(res, "Temperature: %s C", buff);
    display->drawString(x, y, res);

    dtostrf(sensorsController->getHumidity(), 2, 1, buff);
    sprintf(res, "Humidity: %s%%", buff);
    display->drawString(x, y + 10, res);
}

void ScreenCarousel::drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    char res[24];
    char buff[5];
    dtostrf(sensorsController->getPressure(), 4, 1, buff);
    sprintf(res, "Pressure: %s hPa", buff);
    display->drawString(x, y, res);

    dtostrf(sensorsController->getSoilMoisture(), 2, 1, buff);
    sprintf(res, "Soil moisture: %s%%", buff);
    display->drawString(x, y+10, res);

    dtostrf(sensorsController->getLightIntensity(), 2, 1, buff);
    sprintf(res, "Light: %s%%", buff);
    display->drawString(x, y + 20, res);
 }

 int ScreenCarousel::update(){
     return ui.update();
 }