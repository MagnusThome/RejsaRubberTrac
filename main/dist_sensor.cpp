#include <Arduino.h>
#include "dist_sensor.h"
#include "Configuration.h"
#include "algo.h"
#include <Wire.h>

bool DistSensor::initialise(TwoWire *I2Cpipe, char *wheelPos) {
  distance = 0;
  thisWire = I2Cpipe;
  thisWheelPos = wheelPos;
//  Wire.begin(GPIOSDA,GPIOSCL); // initialize I2C w/ I2C pins from config
  pinMode(GPIODISTSENSORXSHUT, OUTPUT);
  digitalWrite(GPIODISTSENSORXSHUT, LOW);
  delay(10);
  digitalWrite(GPIODISTSENSORXSHUT, HIGH);
  delay(10);
  present = sensor.init(true, thisWire);
  sensor.setTimeout(500);
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
