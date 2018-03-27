# PlantNurseSuperMegaMax

Utrecht university interaction technology project

## Installation

First download the zip or clone the project to your machine.

### 1. Setting up the NodeRed server

1. Run `npm install` in /NodeRedPlantNurse
2. To Run the NodeRed server, run `npm run dev`

### 2. Adding credentials for your MQTT server

1. Copy src/CredentialsExample.h and rename the copy to Credentials.h
2. Fill in your server details in Credentials.h
3. You can now upload the project to your device.​

## Pin mapping

| Pin  | Use                                                          |      |      |
| ---- | ------------------------------------------------------------ | ---- | ---- |
| D0   |                                                              |      |      |
| D1   | Servo                                                        |      |      |
| D2   | Selector pin for Amux-board                                  |      |      |
| D3   | Built in Flash button                                        |      |      |
| D4   |                                                              |      |      |
| D5   | I2C SDA                                                      |      |      |
| D6   | I2C SCL                                                      |      |      |
| D7   |                                                              |      |      |
| D8   |                                                              |      |      |
| A0   | Multiplexed output for soil moisture sensor and light sensor. |      |      |