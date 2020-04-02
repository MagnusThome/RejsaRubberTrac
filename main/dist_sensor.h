#include "VL53L0X-2xI2C.h"
#include <Wire.h>
#include "Sensor.h"

class SuspensionTravel : public Sensor
{
  public:
    int16_t distance;
    
    virtual boolean initialise(TwoWire *thisI2c = &Wire, char *wheelPos = NULL, int refrate = -1);
    virtual boolean isConnected();
    virtual void measure();

  protected:
    const byte sensorAddress = byte(0b0101001);

  private:
    VL53L0X sensor;
    static bool xshutInitialized; // both distance sensors are connected to the same XSHUT GPIO => static property
    bool present = false;
  
    static const uint8_t filterSz = 8;
    int16_t filterArr[filterSz];
    
    int16_t distanceFilter(int16_t distanceIn);
};
