#include "WateringController.h"

WateringController::WateringController(uint8_t _servoPin, SSD1306* _oled) {
	servo.attach(_servoPin);
	oled = _oled;
}

void WateringController::startWatering() {
	servo.write(90);
	lastWatering = millis();
	Serial.println("started watering");
	oled->clear();
	oled->setFont(ArialMT_Plain_24);
	oled->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	oled->drawString(oled->width() / 2, oled->height() / 2 - 12, "Watering!");
	isWatering = true;
}

void WateringController::stopWatering() {
	servo.write(0);
	oled->setFont(ArialMT_Plain_10);
	oled->setTextAlignment(TEXT_ALIGN_LEFT);
	Serial.println("started watering");
	isWatering = false;
}

void WateringController::update(){
	if(millis() - lastWatering > waterFrequency /* replace by real condition later*/){
		startWatering();
	}else if(millis() - lastWatering > wateringDuration && isWatering){
		stopWatering();
	}else if(isWatering){
		oled->drawProgressBar(12, 40, 100, 8, 100.0 * (float) (millis() - lastWatering) / (float) wateringDuration);
	}
}