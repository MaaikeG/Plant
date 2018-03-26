#include "ScreenCarousel.h"

ScreenCarousel::ScreenCarousel(SSD1306* _oled, SensorsController* _sensorsController, WateringController* _wateringController):
    ui(_oled)
{
    sensorsController = _sensorsController;
    wateringController = _wateringController;
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

void ScreenCarousel::drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    display->drawString(x, y, "Time since last watering:");

    char res[32];
    unsigned long timeSinceLastWatering = (millis() - wateringController->lastWatering) / 1000;
    uint8_t days = elapsedDays(timeSinceLastWatering);
    uint8_t hours = numberOfHours(timeSinceLastWatering);
    uint8_t minutes = numberOfMinutes(timeSinceLastWatering);    

    sprintf(res, "%d days, %d hours and %d minutes", days, hours, minutes);
    display -> drawStringMaxWidth(x +10, y+8, 100, res);

    if (wateringController->reservoirEmpty) {
        display->drawStringMaxWidth(x, y+32, 128, "Reservoir is empty! Please refill now!");
    }
}

 int ScreenCarousel::update(){
     return ui.update();
 }