# PlantNurseSuperMegaMax

Utrecht university interaction technology project

## Installation

1. Download the zip and extract it
2. Download face dataset (pictures and corresponding masks) from https://surfdrive.surf.nl/files/index.php/s/7d2647093409cec90119a0bea4fcc779
3. Place images and masks in a folder called 'Face_Dataset' in the same folder as the .exe (if you cloned this repo, this should be in {project-root}/SkinAndGestures/x64/{Debug|Release}).
4. Put images and masks in separate folders named 'Photos' (skin color photo's) and 'Masks' respectively.
5. Run `npm install` in /NodeRedPlantNurse
6. To Run the NodeRed server, run `npm run dev`
7. Open the NodeRed interface on http://127.0.0.1:1880/admin
8. At the top change the path of the wfwatch node to the absolute path to {project-root}/SkinAndGestures/SkinAndGestures/commands on your system.
9. Change the value of the Search for field in the Remove filepath node that is connected to the wfwatch node to the same path as above.

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
