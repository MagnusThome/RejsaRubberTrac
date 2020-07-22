#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <Arduino.h>
#include "Configuration.h"
#include "config.h"
#include <Wire.h>
#include "MLX90621.h"
#include "MLX90640.h"

#define ABS_ZERO -2732

typedef struct {
  float  avgFrameTemp = 0; // after normalizing to one row; full width = avg(calculateColumnTemperature(FIS_X))
  float  stdDevFrameTemp = 0; // after normalizing to one row; full width = avg(FIS_X)
  float  avgMinFrameTemp = 0; // the calculatory minimum temp row for the entire full width frame = avg(min(FIS_X))
  float  avgMaxFrameTemp = 0; // the calculatory maximum temp row for the entire full width frame = avg(max(FIS_X))

  float  avgTireTemp = 0; // 0 if no valid autozoom frame
  float  avgOuterTireTemp = 0; // 1/3 of outermost tire pixels; 0 if no valid autozoom frame 
  float  avgMiddleTireTemp = 0; // 1/3 of middle tire pixels; 0 if no valid autozoom frame
  float  avgInnerTireTemp = 0; // 1/3 of innermost tire pixels; 0 if no valid autozoom frame
  float  avgOuterAmbientTemp = 0; // 0 if no valid autozoom frame
  float  avgInnerAmbientTemp = 0; // 0 if no valid autozoom frame
} avgTemps_t; // all temps in degrees Celsius x 10


class TireTreadTemperature
{
  public:
    fis_t*    config;
    status_t* status;

// 2do: document contents better
    int16_t measurement[32];
    int16_t measurement_slope[31]; // How much warmer is the next-inner pixel? Slope of delta temperatures from next-inner to current pixel (measurement[i+1]-measurement[i]); outer = left = array index 0
    int16_t measurement_16[16];
    int16_t measurement_32[32];
    int16_t  picture[128];
    uint16_t pictureOffset;
    boolean validAutozoomFrame = false;
    uint8_t outerTireEdgePosRaw; // The array index of the first pixel _on_ the tire as detected for this frame; corresponds to Max value in the Slope; this index value will be used as input for smoothing; outer = left = array index 0
    uint8_t innerTireEdgePosRaw; // The array index of the last pixel _on_ the tire as detected for this frame; corresponds to Min value in the Slope; this index value will be used as input for smoothing; inner = right = array index FIS_X
    float outerTireEdgePos; // The array index of the first pixel _on_ the tire as smoothed/filtered from the previous few valid autozoom frames; this smoothed index value is used to determine the zoomed tire for this frame; outer = left = array index 0
    float innerTireEdgePos; // The array index of the last pixel _on_ the tire as smoothed/filtered from the previous few valid autozoom frames; this smoothed index value is used to determine the zoomed tire for this frame; inner = right = array index FIS_X
    uint8_t   effective_rows;

    avgTemps_t avgsThisFrame;
    uint16_t totalOutliersThisFrame = 0;
  
    // Let's keep track of some history of this session...
    double totalFrameCount = 0.0; // total number of frames processed in this session
    float runningAvgOutlierRate = 0.0; // running average of outliers detected of this session as percentage of total pixels of the zoomed frame
    float runningAvgZoomedFramesRate = 0.0; // running average of autozoomed frames (i.e. tire detected) of this session as percentage of all frames processed
    float movingAvgFrameTmp = 0.0; // exponential moving average of the unzoomed frame temperature after normalizing to one row (init value = 40 degrees Celsius)
    float movingAvgStdDevFrameTmp = 0.0; // exponential moving average of the unzoomed frame temperature standard deviations after normalizing to one row
    float movingAvgRowDeltaTmp = 0.0; // exponential moving average of the delta between all lowest row values vs. all highest row values
    float maxRowDeltaTmp = 0.0; // maximum of the moving average (movingAvgRowDeltaTmp) to detect shaded rows through increasing deltas with increasingly warm tire temps vs. constantly cold bodywork
    
    boolean initialise(fis_t* _config, status_t* _status, TwoWire *thisI2c = &Wire, char *wheelPos = NULL);
    void measure();
    
  private:
    FISDevice* thisFISDevice;
    TwoWire *thisWire;
  
    float minTempStdDev; // minimum standard deviation we use for outlier detection (25 degrees Celsius)
    float tempTriggerDeltaAmbientTire; // delta temperature threshold for detecting a tire edge
    float tempAvgDeltaAmbientTire; // tire needs to be significantly warmer than ambient temp

    int16_t calculateColumnTemperature(int16_t column_content[], uint8_t size);
    void interpolate(uint8_t startColumn, uint8_t endColumn, int16_t result[]);
    void calculateSlope(int16_t result[]);
    void getMinMaxSlopePosition();
    boolean checkAutozoomValidityAndSetAvgTemps();
    int16_t getMaximum(int16_t arr[], int size);
    int16_t getMinimum(int16_t arr[], int size);
    float getAverage(int16_t arr[], int size);
    float getGeometricMean(int16_t arr[], int size);
    float getStdDev(int16_t arr[], int size);
    float cumulativeProbability(float val, float avg, float stdDev);
    uint16_t removeOutliersChauvenet(int16_t *arr, int size);
};

#endif