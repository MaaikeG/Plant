#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

void setupOLED() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
}

void clearDisplay() {
  display.clearDisplay();
}

void writeText(char text[], int x, int y) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(x,y);
  display.println(text);
  display.display();
}

