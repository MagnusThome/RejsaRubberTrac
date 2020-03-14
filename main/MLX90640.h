#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include "Arduino.h"
#include <Wire.h>

#define TA_SHIFT 8 //Default shift for MLX90640 in open air

class MLX90640 {
private:
  byte refreshRate;        
  byte MLX90640_address;
  float temperatures[768]; //Contains the calculated temperatures of each pixel in the array
  float Tambient;          //Tracks the changing ambient temperature of the sensor
  float Vdd;               //Tracks ... well, Vdd.
  paramsMLX90640 mlx90640;
  TwoWire *i2c;
public:
  boolean isConnected();
  void measure(bool);
  float getTemperature(int num);
  void initialise(int refrate, TwoWire *thisI2c = &Wire);
};
