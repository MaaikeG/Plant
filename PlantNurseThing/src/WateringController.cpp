#include "WateringController.h"

WateringController::WateringController(uint8_t _servoPin,
                                       uint8_t _reservoirEmptyLedPin,
                                       SSD1306& _oled,
                                       void (*_onReservoirEmpty)(),
                                       void (*_onReservoirFilled)())
    : oled(_oled) {
  servoPin = _servoPin;
  reservoirEmptyLedPin = _reservoirEmptyLedPin;
  onReservoirEmpty = _onReservoirEmpty;
  onReservoirFilled = _onReservoirFilled;
}

void WateringController::setAngle(uint16_t newAngle) {
  servo.write(newAngle);
}

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

void WateringController::stopWatering() {
  slowStopTicker.attach_ms(STOP_WATERING_UPDATE_PERIOD, decreaseAngle, this,
                           STOP_WATERING_DURATION);
  oled.setFont(ArialMT_Plain_10);
  oled.setTextAlignment(TEXT_ALIGN_LEFT);
  lastWatering = millis();
  isWatering = false;
}

void WateringController::begin() {
  servo.attach(servoPin);
  servo.write(NO_WATER_FLOW_ANGLE);
  pinMode(reservoirEmptyLedPin, OUTPUT);
}

void WateringController::update(uint8_t soilMoisture) {
  if (isWatering) {
    if (millis() - wateringStart > wateringDuration) {
      stopWatering();
      reservoirEmptyCheckDone = false;
      Serial.println("stopped watering");
    } else {
      oled.drawProgressBar(
          12, 40, 100, 8,
          100.0 * (float)(millis() - wateringStart) / (float)wateringDuration);
    }
    oled.display();
  } else {
    checkReservoirEmpty(soilMoisture);

    if (shouldWater(soilMoisture)) {
      startWatering();
      Serial.println("started watering");
    }
  }
}

bool WateringController::shouldWater(uint8_t soilMoisture) {
  return !reservoirEmpty && reservoirEmptyCheckDone &&
         soilMoisture < soilMoistureThreshold;
}

void WateringController::checkReservoirEmpty(uint8_t soilMoisture) {
  // do this check only once after giving the water time to spread to the
  // sensor.
  if (!reservoirEmptyCheckDone &&
      millis() - lastWatering > reservoirEmptyTimeCheck) {
    if (soilMoisture < soilMoistureThreshold && !reservoirEmpty) {
      reservoirEmpty = true;
      digitalWrite(reservoirEmptyLedPin, HIGH);
      onReservoirEmpty();
    } else if (reservoirEmpty) {
      reservoirEmpty = false;
      digitalWrite(reservoirEmptyLedPin, LOW);
      onReservoirFilled();
    }
    reservoirEmptyCheckDone = true;
  }
}