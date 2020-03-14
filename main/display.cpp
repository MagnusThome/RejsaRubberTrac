#include "display.h"
#include "Configuration.h"
#include "algo.h"

void Display::setup() {
  displayDevice = Adafruit_SSD1306(128, 32, &Wire);
  displayDevice.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  displayDevice.display();
  delay(1000);
  displayDevice.clearDisplay();
}

void Display::refreshDisplay(float tempMeasurement[], float leftEdge, float rightEdge, boolean validFrame, float updateRate, uint32_t distance, uint8_t percentage, boolean bleConnected) {
  displayDevice.clearDisplay();
  drawBarChart(tempMeasurement,44,0,2,28, leftEdge, rightEdge);
  displayDevice.setCursor(0,8);
  displayDevice.printf("%d%%\n%dmm\n%.0fHz", percentage, distance, updateRate);
  if (validFrame) displayDevice.fillCircle(3, 3, 3, WHITE);
  else displayDevice.drawCircle(3, 3, 3, WHITE);
  if (bleConnected) displayDevice.fillCircle(11, 3, 3, WHITE);
  else displayDevice.drawCircle(11, 3, 3, WHITE);
  displayDevice.display();
}

void Display::drawBarChart(float tempMeasurement[], int x, int y, int barWidth, int h, float leftEdge, float rightEdge) {
  float minTemp = tempMeasurement[0];
  float maxTemp = tempMeasurement[0];
  for (uint8_t i=0;i<FIS_X;i++) {
    if (tempMeasurement[i] > maxTemp) maxTemp = tempMeasurement[i];
    if (tempMeasurement[i] < minTemp) minTemp = tempMeasurement[i];
  }
  minTemp=200.0;
  if (maxTemp<300.0) maxTemp=300.0;
  displayDevice.drawPixel(x+(int16_t)leftEdge*2, 30, WHITE);
  displayDevice.drawPixel(x+(int16_t)leftEdge*2, 31, WHITE);
  displayDevice.drawPixel(x+(int16_t)rightEdge*2, 30, WHITE);
  displayDevice.drawPixel(x+(int16_t)rightEdge*2, 31, WHITE);
  float stepSize = (maxTemp-minTemp)/(float)h;
  for (uint8_t i=0;i<FIS_X;i++) {
    for (uint8_t j=0;j<barWidth;j++) displayDevice.drawLine(x+j+i*barWidth,h-1+y,x+j+i*barWidth,h-1-(int)((tempMeasurement[i]-minTemp)/stepSize)+y, WHITE);
  }
  displayDevice.setTextSize(1);
  displayDevice.setTextColor(WHITE);
  displayDevice.setCursor(x+FIS_X*barWidth+1,y);
  displayDevice.printf("%.0fC", maxTemp/10);
  displayDevice.setCursor(x+FIS_X*barWidth+1,y+h-3);
  displayDevice.printf("%.0fC" ,minTemp/10);
  
}
