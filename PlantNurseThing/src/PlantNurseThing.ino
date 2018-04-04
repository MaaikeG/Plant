#include <FS.h>  //this needs to be first, or it all crashes and burns...

#include <SSD1306.h>
#include <Ticker.h>
#include <Wire.h>
#include "DebouncedButton.h"
#include "ManagedWiFiClient.h"
#include "MqttClient.h"
#include "ScreenCarousel.h"
#include "SensorsController.h"
#include "WateringController.h"

#define SERVO_PIN D1
#define AMUX_SELECTOR_PIN D2
#define FLASH_BUTTON_PIN D3
#define I2C_SDA D5
#define I2C_SCL D6
#define RESERVOIR_EMPTY_LED D7
#define AMUX_OUTPUT_PIN A0

#define SENSOR_UPDATE_PERIOD 2000
#define SENSOR_PUBLISH_PERIOD 60000

SensorsController sensorsController(AMUX_OUTPUT_PIN, AMUX_SELECTOR_PIN);
SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
void onReservoirEmpty(); // forward declarations
void onReservoirFilled();
WateringController wateringController(SERVO_PIN, RESERVOIR_EMPTY_LED, oled, onReservoirEmpty, onReservoirFilled);
ScreenCarousel screenCarousel(oled, sensorsController, wateringController);
Ticker updateSensorValuesTicker;
bool updateSensorValuesNextIteration = true;
Ticker publishSensorValuesTicker;
bool publishSensorValuesNextIteration = true;

enum Mode { Manual, Automatic };

FrameCallback frames[] = {
    [](OLEDDisplay* display, OLEDDisplayUiState* state, short x, short y) {
      screenCarousel.drawFrame1(display, state, x, y);
    },
    [](OLEDDisplay* display, OLEDDisplayUiState* state, short x, short y) {
      screenCarousel.drawFrame2(display, state, x, y);
    },
    [](OLEDDisplay* display, OLEDDisplayUiState* state, short x, short y) {
      screenCarousel.drawFrame3(display, state, x, y);
    }};

Mode currentMode;

DebouncedButton modeToggleButton(FLASH_BUTTON_PIN);
bool modeToggled = true;

ManagedWiFiClient managedWiFiClient;
MqttClient* mqttClient;

void setup() {
  Serial.begin(9600);
  oled.init(); // wire.begin is called here
  wateringController.begin();
  sensorsController.begin();

  updateSensorValuesTicker.attach_ms(SENSOR_UPDATE_PERIOD, []() {
    // set a flag because actually running it takes too long, because at the
    // moment we water and the same time, because we don't have a better way to
    // activate that yet
    updateSensorValuesNextIteration = true;
  });
  publishSensorValuesTicker.attach_ms(
      SENSOR_PUBLISH_PERIOD, []() { publishSensorValuesNextIteration = true; });

  pinMode(LED_BUILTIN, OUTPUT);
  setMode(Manual);
  modeToggleButton.begin();

  screenCarousel.begin(frames, 3);
  oled.flipScreenVertically();

  WiFiManager wiFiManager;
  // reset settings - for testing
  //wiFiManager.resetSettings();
  mqttClient = new MqttClient(managedWiFiClient, wiFiManager);
  mqttClient->addParameters();
  managedWiFiClient.begin(wiFiManager, [](WiFiManager* wiFiManager) {
    configModeCallback(wiFiManager, &oled);
  });
  mqttClient->saveParameters();

  mqttClient->begin();
  mqttClient->subscribe(TEST_TOPIC, 1);
  mqttClient->subscribe(WATERING_TOPIC, 1);
  mqttClient->subscribe(MODE_TOGGLE_TOPIC, 1);
  mqttClient->subscribe(UPDATE_SENSOR_TOPIC, 1);
  mqttClient->setCallback(messageCallback);
}

void loop() {
  if (currentMode == Automatic || wateringController.isWatering) {
    wateringController.update(sensorsController.getSoilMoisture());
  }

  if (!wateringController.isWatering) {
    int remainingTimeBudget = screenCarousel.update();
    void (*todos[]) () = {
      []() {
        if (updateSensorValuesNextIteration) {
          updateSensorValuesNextIteration = false;
          sensorsController.updateSensorValues();
        }
      },
      []() {
        if(publishSensorValuesNextIteration){
          publishSensorValuesNextIteration = false;
          mqttClient->publishSensorValues(sensorsController);
        }
      }
    };
    unsigned long start = millis();
    for (auto const& todo : todos) {
      if(millis() - remainingTimeBudget > start){
        todo();
      }else
        break;
    }
  }

  if (modeToggleButton.read() == LOW) {
    if (!modeToggled) {
      Mode newMode = currentMode == Automatic ? Manual : Automatic;
      mqttClient->publish(MODE_TOGGLE_TOPIC, newMode == Automatic ? "automatic" : "manual", (boolean) true);
      setMode(newMode);
    }
  } else {
    modeToggled = false;
  }
  
  mqttClient->update();
}

void setMode(Mode mode) {
  currentMode = mode;
  digitalWrite(LED_BUILTIN, mode == Automatic ? LOW : HIGH);
  modeToggled = true;
}

void messageCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(TEST_TOPIC, topic) == 0) {
    Serial.print("test: ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
  }else if(strcmp(WATERING_TOPIC, topic) == 0){
    wateringController.startWatering();
  }else if(strcmp(UPDATE_SENSOR_TOPIC, topic) == 0){
    updateSensorValuesNextIteration = true;
    publishSensorValuesNextIteration = true;
  }else if(strcmp(MODE_TOGGLE_TOPIC, topic) == 0){
    payload[length] = '\0';
    String payloadString = String((char*)payload);
    if(payloadString == "manual"){
      setMode(Manual);
    }else if(payloadString == "automatic"){
      setMode(Automatic);
    }else{
      Serial.println("Invalid setMode payload!");
    }
  }
}

void onReservoirEmpty(){
  mqttClient->publish(WATER_RESERVOIR_EMPTYNESS_TOPIC, "empty", 1);
}

void onReservoirFilled(){
  mqttClient->publish(WATER_RESERVOIR_EMPTYNESS_TOPIC, "not empty", 1);
}