#include "VL53L0X-2xI2C.h"
#include <Wire.h>

class DistSensor {
private:
  boolean present;
  VL53L0X sensor;
  TwoWire *thisWire;
  char *thisWheelPos;
public:
  int16_t distance;
  bool initialise(TwoWire *I2Cpipe = &Wire, char *wheelPos = NULL);
  void measure(void);
};
