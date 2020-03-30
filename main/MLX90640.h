#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include "Arduino.h"
#include <Wire.h>

#define TA_SHIFT 8 // default shift for MLX90640 in open air
#define MLX90640_ADDRESS 0x33

class MLX90640 {
private:
  byte refreshRate;        
  float temperatures[768]; //Contains the calculated temperatures of each pixel in the array
  float Tambient;          //Tracks the changing ambient temperature of the sensor
  float Vdd;               //Tracks ... well, Vdd.
  paramsMLX90640 mlx90640;
  TwoWire *i2c;
public:
  boolean isConnected();
  void measure(bool);
  float getTemperature(int num);
  boolean initialise(int refrate, TwoWire *thisI2c = &Wire);
};
