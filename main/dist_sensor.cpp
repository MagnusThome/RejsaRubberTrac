#include <Arduino.h>
#include "dist_sensor.h"
#include "Configuration.h"
#include <Wire.h>

bool DistSensor::xshutInitialized = false;

bool DistSensor::initialise(TwoWire *I2Cpipe, char *wheelPos) {
  distance = 0;
  thisWire = I2Cpipe;
  thisWheelPos = wheelPos;

  if (!xshutInitialized) {
    Serial.print("Resetting XSHUT GPIO for (possibly both) distance sensors (at once).\n");
    // only to be done once, since both distance sensors are connected to the same XSHUT GPIO
    pinMode(GPIODISTSENSORXSHUT, OUTPUT);
    digitalWrite(GPIODISTSENSORXSHUT, LOW);
    delay(10);
    digitalWrite(GPIODISTSENSORXSHUT, HIGH);
    delay(10);
    xshutInitialized = true;
  }

  sensor.setTimeout(500);
  present = sensor.init(true, thisWire);
  sensor.startContinuous();

  return present;
};

void DistSensor::measure() {
  if (present) {
    int16_t rawDistance = sensor.readRangeContinuousMillimeters();
    if (rawDistance < 500) {
       distance = distanceFilter(rawDistance) - DISTANCEOFFSET; // Only update distance if it is less than 500mm
    }
    if (sensor.timeoutOccurred()) {
      Serial.print("TIMEOUT: Distance sensor for ");
      Serial.print(thisWheelPos);
      Serial.print("!\n");
    }
  }
}

int16_t DistSensor::distanceFilter(int16_t distanceIn) {
  int16_t distanceOut = 0;
  for (int8_t i=0; i<(filterSz-1); i++) {
    filterArr[i] = filterArr[i+1];
    distanceOut += filterArr[i+1];
  }
  filterArr[filterSz-1] = distanceIn;
  distanceOut += distanceIn;
  return (int16_t) distanceOut/filterSz;
}
