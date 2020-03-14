#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display {
private:
  Adafruit_SSD1306 displayDevice;
  void drawBarChart(float tempMeasurement[], int x, int y, int barWidth=2, int h=32, float leftEdge=0, float rightEdge=32);
public:
  void setup();
  void refreshDisplay(float tempMeasurement[], float leftEdge, float rightEdge, boolean validFrame, float updateRate, uint32_t distance, uint8_t percentage, boolean bleConnected);
};
