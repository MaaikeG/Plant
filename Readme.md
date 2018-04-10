# PlantNurseSuperMegaMax

Utrecht university interaction technology project

## Installation

First download the zip or clone the project to your machine.

### Setting up the NodeRed server

1. Run `npm install` in /NodeRedPlantNurse
2. To Run the NodeRed server, run `npm run dev`
3. Open the NodeRed interface on http://127.0.0.1:1880/admin
4. Set up a your MQTT broker by clicking on a MQTT node and adding a new server config

### Setting up the device

1. When the device runs the code for the first time, it will create a WiFi you can connect to. The name and password will be displayed on the OLED screen.
2. Upon connection you have to "log in" to the access point and it will open a web interface in which you can fill in the credentials for your WiFi network
3. After saving the credentials, the device will attempt to connect to the Wifi network you configured and shut down the access point or reboot and show the access point again if that fails.

## Pin mapping

| Pin  | Use                                                          |      |      |
| ---- | ------------------------------------------------------------ | ---- | ---- |
| D0   |                                                              |      |      |
| D1   | Servo                                                        |      |      |
| D2   | Selector pin for Amux-board                                  |      |      |
| D3   | Built in Flash button                                        |      |      |
| D4   | Built in LED                                                 |      |      |
| D5   | I2C SDA                                                      |      |      |
| D6   | I2C SCL                                                      |      |      |
| D7   | Reservoir empty led                                          |      |      |
| D8   |                                                              |      |      |
| A0   | Multiplexed output for soil moisture sensor and light sensor. |      |      |