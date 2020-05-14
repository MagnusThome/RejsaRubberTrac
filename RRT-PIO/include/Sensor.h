#ifndef Sensor_h
#define Sensor_h

#include "Arduino.h"
#include <Wire.h>

class Sensor {
  public:
    virtual boolean initialise(TwoWire *thisI2c = &Wire, char *wheelPos = NULL, int refrate = -1) = 0;
    virtual boolean isConnected() = 0;
    virtual void measure() = 0;

    // Returns the sensor refresh rate since the last call of getRefreshRate().
    float getRefreshRate() {
      float refreshRate = measurementCycles / (millis()-lastRefreshRateUpdate) * 1000;
      lastRefreshRateUpdate = millis();
      measurementCycles = 0.0;
      return refreshRate;
    };

  protected:
    const byte sensorAddress = byte(0x00);
    TwoWire *i2c;
    char *thisWheelPos;
    long lastRefreshRateUpdate = 0;
    float measurementCycles = 0.0; // Counts how many measurement cycles were completed since last update of the refresh rate. Needs to be updated in the measure() function.
};


class FISDevice : public Sensor {
  public:
    virtual float getPixelTemperature(uint8_t x, uint8_t y) = 0;

  protected:
    virtual float getTemperature(int num) = 0;
};

#endif
