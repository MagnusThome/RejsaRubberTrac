#include "temp_sensor.h"
#include "Configuration.h"
#include "algo.h"
#include <Arduino.h>
#include <Wire.h>
#include "spline.h"

void TempSensor::initialise(int refrate, TwoWire *I2Cpipe) {
  rawMinSlopePosition= FIS_X;
  rawMaxSlopePosition= 0;
  leftEdgePosition = 0.0;
  rightEdgePosition = (float)FIS_X;
  thisWire = I2Cpipe;
  FISDevice.initialise(refrate, thisWire);
};

void TempSensor::measure() {
  uint16_t column_content[EFFECTIVE_ROWS];
  FISDevice.measure(true);
  for(uint8_t x=0;x<FIS_X;x++){
    for (uint8_t y=0;y<EFFECTIVE_ROWS;y++) { // Read the columns first
      column_content[y] = getPixelTemperature(x, y);
    }
    measurement[x]=calculateColumnTemperature(column_content, EFFECTIVE_ROWS);
  }
#ifdef FIS_AUTORANGING
  calculateSlope(measurement_slope);
  getMinMaxSlopePosition();
  validAutorangeFrame = checkAutorangeValidity();
  if (validAutorangeFrame) {
    float leftStepSize = abs((rawMaxSlopePosition-leftEdgePosition)/4);
    float rightStepSize = abs((rawMinSlopePosition-rightEdgePosition)/4);
    if (leftEdgePosition < rawMaxSlopePosition) leftEdgePosition += leftStepSize;
    else leftEdgePosition -= leftStepSize;
    if (rightEdgePosition < rawMinSlopePosition) rightEdgePosition += rightStepSize;
    else rightEdgePosition -= rightStepSize;
    if (rightEdgePosition < 0) leftEdgePosition = 0;
    if (leftEdgePosition < 0) rightEdgePosition = 0;
    if (leftEdgePosition > FIS_X) leftEdgePosition = 32;
    if (rightEdgePosition > FIS_X) rightEdgePosition = 32;
  }
#endif
  interpolate((int)leftEdgePosition, (int)rightEdgePosition, measurement_16);
}

float TempSensor::getPixelTemperature(uint8_t x, uint8_t y) {
#if FIS_SENSOR == FIS_MLX90621
  return FISDevice.getTemperature((y+IGNORE_TOP_ROWS+x*FIS_Y) + TEMPOFFSET) * 10 * TEMPSCALING; // MLX90621 iterates in columns
#elif FIS_SENSOR == FIS_MLX90640
  return FISDevice.getTemperature((y*FIS_X+IGNORE_TOP_ROWS*FIS_X+x) + TEMPOFFSET) * 10 * TEMPSCALING; // MLX90640 iterates in rows
#endif
}

float TempSensor::calculateColumnTemperature(uint16_t column_content[], uint8_t size) {
#if COLUMN_AGGREGATE == COLUMN_AGGREGATE_MAX
  return get_maximum(column_content, size);
#elif COLUMN_AGGREGATE == COLUMN_AGGREGATE_AVG
  return get_average(column_content, size);
#endif
}

void TempSensor::interpolate(uint8_t startColumn, uint8_t endColumn,int16_t result[]) {
  float stepSize = (endColumn-startColumn)/16.0;
  float x[32];
  for (uint8_t i=0;i<FIS_X;i++) x[i]=i; // Initialize the X axis of an array {0, 1, 2 ... 30, 31}
  Spline linearSpline(x,measurement,FIS_X,1);
  for (uint8_t i=0;i<16;i++) result[i] = linearSpline.value(startColumn+i*stepSize);
}

void TempSensor::calculateSlope(float result[]) {
  for (uint8_t i=0;i<FIS_X-1;i++) result[i] = measurement[i+1]-measurement[i];
}

void TempSensor::getMinMaxSlopePosition() {
  float minSlopeValue = 0;
  float maxSlopeValue = 0;
  for (uint8_t i=0;i<FIS_X-1;i++) {
    if (measurement_slope[i] > maxSlopeValue ) {
      maxSlopeValue = measurement_slope[i];
      rawMaxSlopePosition = i;
    }
    if (measurement_slope[i] < minSlopeValue ) {
      minSlopeValue = measurement_slope[i];
      rawMinSlopePosition = i;
    }
  }
}

boolean TempSensor::checkAutorangeValidity() {
  float avgTireTemp=0;
  float avgAmbientTemp=0;
  if (measurement_slope[rawMinSlopePosition] > -7) return false;
  if (measurement_slope[rawMaxSlopePosition] < 7) return false;
  if (rawMinSlopePosition < rawMaxSlopePosition) return false; // Inner or outer edge of tire out of camera view
  if ((rawMinSlopePosition-rawMaxSlopePosition) < AUTORANGING_MINIMUM_TIRE_WIDTH) return false; // Too thin tire
  for (uint8_t i=0;i<FIS_X;i++) {
    if (i < rawMaxSlopePosition || i > rawMinSlopePosition ) avgAmbientTemp += measurement[i];
    else avgTireTemp += measurement[i];
  }
  avgTireTemp = avgTireTemp / (rawMinSlopePosition-rawMaxSlopePosition);
  avgAmbientTemp = avgAmbientTemp / (rawMaxSlopePosition + (FIS_X-rawMinSlopePosition));
  if (avgTireTemp - avgAmbientTemp < 5.0) return false; // Tire is not significantly hotter than ambient
  return true;
}
