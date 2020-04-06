#include <Arduino.h>
#include "dist_sensor.h"
#include "Configuration.h"
#include <Wire.h>

bool SuspensionTravel::xshutInitialized = false;

boolean SuspensionTravel::initialise(TwoWire *thisI2c, char *wheelPos, int refrate) { // refrate is ignored for this type of sensor
  distance = 0;
  i2c = thisI2c;
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
  present = sensor.init(true, i2c);
  sensor.startContinuous();

  return isConnected();
};

boolean SuspensionTravel::isConnected() {
  return present;
}

void SuspensionTravel::measure() {
  if (isConnected()) {
    uint16_t rawDistance = sensor.readRangeContinuousMillimeters();
    if (rawDistance < 500) {
       distance = distanceFilter((int16_t)rawDistance) - DISTANCEOFFSET; // Only update distance if it is less than 500mm
    }
    if (sensor.timeoutOccurred()) {
      Serial.print("TIMEOUT: Distance sensor for ");
      Serial.print(thisWheelPos);
      Serial.print("!\n");
    }
  } else {
    distance = -1;
  }

  measurementCycles++;
}

int16_t SuspensionTravel::distanceFilter(int16_t distanceIn) {
  int16_t distanceOut = 0;
  for (int8_t i=0; i<(filterSz-1); i++) {
    filterArr[i] = filterArr[i+1];
    distanceOut += filterArr[i+1];
  }
  filterArr[filterSz-1] = distanceIn;
  distanceOut += distanceIn;
  return (int16_t) distanceOut/filterSz;
}
