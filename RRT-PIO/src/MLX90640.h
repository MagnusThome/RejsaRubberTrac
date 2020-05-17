#ifndef MLX90640_H_
#define MLX90640_H_

#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include "Arduino.h"
#include <Wire.h>
#include "Sensor.h"

#define TA_SHIFT 8 // default shift for MLX90640 in open air
#define MLX90640_ADDRESS 0x33

// size
#define MLX90640_X 32  // Far Infrared Sensor columns
#define MLX90640_Y 24  // Far Infrared Sensor rows


class MLX90640 : public FISDevice
{
  public:
	  virtual boolean initialise(TwoWire *thisI2c = &Wire, char *wheelPos = NULL, int refrate = -1);
    virtual boolean isConnected();
    virtual void measure();
    virtual float getPixelTemperature(uint8_t x, uint8_t y);
    virtual float getTemperature(int num);
	  virtual float getAmbient();
    
  protected:
    const byte sensorAddress = byte(MLX90640_ADDRESS);

  private:
    float temperatures[768]; //Contains the calculated temperatures of each pixel in the array
    float Tambient;          //Tracks the changing ambient temperature of the sensor
    float Vdd;               //Tracks ... well, Vdd.
    paramsMLX90640 mlx90640;
};

#endif
