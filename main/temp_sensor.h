#include "MLX90621.h"
#include "MLX90640.h"
#include "Configuration.h"
#include <Arduino.h>
#include <Wire.h>

class TempSensor {
private:
  float getPixelTemperature(uint8_t x, uint8_t y);
  float calculateColumnTemperature(uint16_t column_content[], uint8_t size);
  void interpolate(uint8_t startColumn, uint8_t endColumn, int16_t result[]);
  void calculateSlope(float result[]);
  void getMinMaxSlopePosition();
  boolean checkAutorangeValidity();
#if FIS_SENSOR == FIS_MLX90621
  MLX90621 FISDevice;
#elif FIS_SENSOR == FIS_MLX90640
  MLX90640 FISDevice;
#endif
  TwoWire *thisWire;
public:
  float measurement[FIS_X];
  float measurement_slope[FIS_X];
  boolean validAutorangeFrame;
  int16_t measurement_16[16];
  uint8_t rawMinSlopePosition;
  uint8_t rawMaxSlopePosition;
  float leftEdgePosition;
  float rightEdgePosition;
	void initialise(int refrate, TwoWire *I2Cpipe = &Wire);
	void measure();
};
