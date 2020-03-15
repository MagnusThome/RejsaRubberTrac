#include "Configuration.h"
#include <Arduino.h>
#include <Wire.h>
#if FIS_SENSOR == FIS_MLX90621
  #include "MLX90621.h"
#elif FIS_SENSOR == FIS_MLX90640
  #include "MLX90640.h"
#endif

typedef struct {
  float  avgFrameTemp = 0; // after normalizing to one row; full width = avg(FIS_X)
  float  avgTireTemp = 0; // 0 if no valid autorange
  float  avgOuterTireTemp = 0; // 1/3 of outermost tire pixels; 0 if no valid autorange 
  float  avgMiddleTireTemp = 0; // 1/3 of middle tire pixels; 0 if no valid autorange
  float  avgInnerTireTemp = 0; // 1/3 of innermost tire pixels; 0 if no valid autorange
  float  avgOuterAmbientTemp = 0; // 0 if no valid autorange
  float  avgInnerAmbientTemp = 0; // 0 if no valid autorange
} avgTemps_t; // all temps in degrees Celsius x 10


class TempSensor {
private:
#if FIS_SENSOR == FIS_MLX90621
  MLX90621 FISDevice;
#elif FIS_SENSOR == FIS_MLX90640
  MLX90640 FISDevice;
#endif
  
  TwoWire *thisWire;

  int16_t getPixelTemperature(uint8_t x, uint8_t y);
  int16_t calculateColumnTemperature(int16_t column_content[], uint8_t size);
  void interpolate(uint8_t startColumn, uint8_t endColumn, int16_t result[]);
  void calculateSlope(int16_t result[]);
  void getMinMaxSlopePosition();
  boolean checkAutorangeValidityAndSetAvgTemps();
  int16_t getMaximum(int16_t arr[], int size);
  int16_t getMinimum(int16_t arr[], int size);
  float getAverage(int16_t arr[], int size);
  float getGeometricMean(int16_t arr[], int size);
  float getStdDev(int16_t arr[], int size);
  float cumulativeProbability(float val, float avg, float stdDev);
  uint16_t removeOutliersChauvenet(int16_t *arr, int size);
  
public:
  int16_t measurement[FIS_X];
  int16_t measurement_slope[FIS_X-1];
  boolean validAutorangeFrame = false;
  int16_t measurement_16[16];
  uint8_t outerTireEdgePositionThisFrameViaSlopeMax; // i.e. the index of the first pixel _on_ the tire as detected for this measurement; corresponds to Max value in the Slope
  uint8_t innerTireEdgePositionThisFrameViaSlopeMin; // i.e. the index of the last pixel _on_ the tire as detected for this measurement; corresponds to Min value in the Slope
  float outerTireEdgePositionSmoothed; // outer = left = array index 0
  float innerTireEdgePositionSmoothed; // inner = right = array index FIS_X

  avgTemps_t avgsThisFrame;
  uint16_t totalOutliersThisFrame = 0;

  double totalFrameCount = 0;
  float runningAvgOutlierRatePerFrame = 0;
  float runningAvgZoomedFramesToTotalFramesViaSlope = 0;
  float runningAvgFrameTmp = 0.0; // 2do: init value to be replaced by heuristically determined real life value; also update removeOutliersChauvenet()
  float runningAvgStdDevFrameTmp = 0.0; // 2do: init value to be replaced by heuristically determined real life value; also update removeOutliersChauvenet()

  
	void initialise(int refrate, TwoWire *I2Cpipe = &Wire);
	void measure();
};
