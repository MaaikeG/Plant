#include "WateringController.h"

WateringController::WateringController(uint8_t _servoPin,
                                       uint8_t _reservoirEmptyLedPin,
                                       SSD1306& _oled,
                                       void (*_onReservoirEmpty)(),
                                       void (*_onReservoirFilled)(),
                                       uint8_t (*_getSoilMoisture)())
    : oled(_oled), slowStopTicker(), checkReservoirEmptyTicker() {
  servoPin = _servoPin;
  reservoirEmptyLedPin = _reservoirEmptyLedPin;
  onReservoirEmpty = _onReservoirEmpty;
  onReservoirFilled = _onReservoirFilled;
  getSoilMoisture = _getSoilMoisture;
}

void WateringController::setAngle(uint16_t newAngle) { servo.write(newAngle); }

void WateringController::startWatering() {
  setAngle(WATER_FLOW_ANGLE);
  wateringStart = millis();
  oled.clear();
  oled.setFont(ArialMT_Plain_24);
  oled.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  oled.drawString(oled.width() / 2, oled.height() / 2 - 12, "Watering!");
  isWatering = true;
}

void decreaseAngle(WateringController* wateringController) {
  wateringController->setAngle(WATER_FLOW_ANGLE -
                               (millis() - wateringController->lastWatering) *
                                   (WATER_FLOW_ANGLE - NO_WATER_FLOW_ANGLE) /
                                   STOP_WATERING_DURATION);
}

void checkReservoirEmpty(WateringController* wateringController) {
  if (wateringController->getSoilMoisture() < soilMoistureThreshold) {
    wateringController->reservoirEmptied();
  } else {
    wateringController->reservoirFilled();
  }
  wateringController->reservoirEmptyCheckDone = true;
}

void WateringController::stopWatering() {
  slowStopTicker.attach_ms(STOP_WATERING_UPDATE_PERIOD, decreaseAngle, this,
                           STOP_WATERING_DURATION);
  checkReservoirEmptyTicker.once_ms(reservoirEmptyCheckTime, checkReservoirEmpty, this);
  oled.setFont(ArialMT_Plain_10);
  oled.setTextAlignment(TEXT_ALIGN_LEFT);
  lastWatering = millis();
  isWatering = false;
  reservoirEmptyCheckDone = false;
}

void WateringController::begin() {
  servo.attach(servoPin);
  servo.write(NO_WATER_FLOW_ANGLE);
  pinMode(reservoirEmptyLedPin, OUTPUT);
}

void WateringController::update() {
  if (isWatering) {
    if (millis() - wateringStart > wateringDuration) {
      stopWatering();
    } else {
      oled.drawProgressBar(
          12, 40, 100, 8,
          100.0 * (float)(millis() - wateringStart) / (float)wateringDuration);
      oled.display();
    }
  } else if (shouldWater()) {
    startWatering();
  }
}

bool WateringController::shouldWater() {
  return !reservoirEmpty && reservoirEmptyCheckDone &&
         getSoilMoisture() < soilMoistureThreshold;
}

void WateringController::reservoirEmptied() {
  reservoirEmpty = true;
  digitalWrite(reservoirEmptyLedPin, HIGH);
  onReservoirEmpty();
}

void WateringController::reservoirFilled() {
  reservoirEmpty = false;
  digitalWrite(reservoirEmptyLedPin, LOW);
  onReservoirFilled();
}