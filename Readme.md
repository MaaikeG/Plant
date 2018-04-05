# PlantNurseSuperMegaMax

Utrecht university interaction technology project

## Installation

1. Download the zip and extract it
2. Run `npm install` in /NodeRedPlantNurse
3. To Run the NodeRed server, run `npm run dev`

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

## SkinAndGestures
Download face dataset (pictures and corresponding masks) from https://surfdrive.surf.nl/files/index.php/s/7d2647093409cec90119a0bea4fcc779
Place images and masks in separate folders and adjust paths in line 110 an 115
of SkinAndGestures.cpp accordingly.