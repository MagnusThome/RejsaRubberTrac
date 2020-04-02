#ifndef Sensor_h
#define Sensor_h

#include "Arduino.h"
#include <Wire.h>

class Sensor
{
  public:
    float actualRefreshRate;

    virtual boolean initialise(TwoWire *thisI2c = &Wire, char *wheelPos = NULL, int refrate = -1) = 0;
    virtual boolean isConnected() = 0;
    virtual void measure() = 0;

    void updateActualRefreshRate(void) {
      actualRefreshRate = measurementCycles / (millis()-lastUpdate) * 1000;
      lastUpdate = millis();
      measurementCycles = 0.0;
    };

  protected:
    const byte sensorAddress = byte(0x00);
    TwoWire *i2c;
    char *thisWheelPos;
    float measurementCycles = 0.0; // Counts how many measurement cycles were completed since last update of the actual sensor refresh rate. Needs to be updated in the measure() function
    long lastUpdate = 0;
};


class FISDevice : public Sensor
{
  protected:
    virtual float getTemperature(int num) = 0;
  
  public:
    virtual float getPixelTemperature(uint8_t x, uint8_t y) = 0;
};

#endif
