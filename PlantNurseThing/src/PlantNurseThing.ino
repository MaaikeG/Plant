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

#define I2C_SDA D5
#define I2C_SCL D6

#define SENSOR_UPDATE_PERIOD 2000
#define SENSOR_PUBLISH_PERIOD 10000

SensorsController sensorsController(A0, D2);
SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
WateringController wateringController(D1, oled);
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

DebouncedButton modeToggleButton(D3);
bool modeToggled = true;

ManagedWiFiClient managedWiFiClient;
MqttClient* mqttClient;

void setup() {
  Serial.begin(9600);
  Wire.begin(D5, D6);
  wateringController.begin();

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
  oled.init();
  screenCarousel.begin(frames, 3);
  oled.flipScreenVertically();

  WiFiManager wiFiManager;
  // reset settings - for testing
  // wiFiManager.resetSettings();
  mqttClient = new MqttClient(managedWiFiClient, wiFiManager);
  mqttClient->addParameters();
  managedWiFiClient.begin(wiFiManager, [](WiFiManager* wiFiManager) {
    configModeCallback(wiFiManager, &oled);
  });
  mqttClient->saveParameters();

  mqttClient->begin();
  mqttClient->subscribe(TEST_TOPIC, 1);
  mqttClient->subscribe(WATERING_TOPIC, 1);
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
      setMode(currentMode == Automatic ? Manual : Automatic);
      modeToggled = true;
    }
  } else {
    modeToggled = false;
  }
  
  mqttClient->update();
}

void setMode(Mode mode) {
  currentMode = mode;
  digitalWrite(LED_BUILTIN, mode == Automatic ? LOW : HIGH);
}

void messageCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(TEST_TOPIC, topic) == 0) {
    Serial.print("test: ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
  }else if(strcmp(WATERING_TOPIC, topic) == 0){
    wateringController.startWatering();
  }
}