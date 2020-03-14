#include "VL53L0X-2xI2C.h"
#include <Wire.h>

class DistSensor {
private:
  static bool xshutInitialized;
  bool present = false;
  VL53L0X sensor;
  TwoWire *thisWire;
  char *thisWheelPos;

  static const uint8_t filterSz = 8;
  int16_t filterArr[filterSz];
  
  int16_t distanceFilter(int16_t distanceIn);
public:
  int16_t distance;
  bool initialise(TwoWire *I2Cpipe = &Wire, char *wheelPos = NULL);
  void measure(void);
};
