#include "temp_sensor.h"
#include "Configuration.h"
#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "spline.h"

boolean TireTreadTemperature::initialise(TwoWire *thisI2c, char *wheelPos, int fisRefrate) {
  #if FIS_SENSOR == FIS_MLX90621
    thisFISDevice = new MLX90621();
  #elif FIS_SENSOR == FIS_MLX90640
    thisFISDevice = new MLX90640();
  #else
    return false;
  #endif

  innerTireEdgePositionThisFrameViaSlopeMin = FIS_X;
  outerTireEdgePositionThisFrameViaSlopeMax = 0;
  outerTireEdgePositionSmoothed = 0.0;
  innerTireEdgePositionSmoothed = (float)FIS_X;
  thisWire = thisI2c;
  return thisFISDevice->initialise(thisWire, wheelPos, fisRefrate);
};

void TireTreadTemperature::measure() {
  int16_t column_content[EFFECTIVE_ROWS];
  float avgMins = 0.0;
  float avgMaxs = 0.0;

  totalFrameCount++;
  totalOutliersThisFrame = 0;
  thisFISDevice->measure();
  
  for (uint8_t x=0; x<FIS_X; x++){
    for (uint8_t y=0; y<EFFECTIVE_ROWS; y++) { // Read the columns first
      column_content[y] = (int16_t)thisFISDevice->getPixelTemperature(x, y);
    }
    measurement[x] = calculateColumnTemperature(column_content, EFFECTIVE_ROWS);
    avgMins = ((float)getMinimum(column_content, EFFECTIVE_ROWS) - avgMins) / (x+1);
    avgMaxs = ((float)getMaximum(column_content, EFFECTIVE_ROWS) - avgMaxs) / (x+1);
  }

  // reset average temperatures
  avgsThisFrame.avgFrameTemp = 0.0;
  avgsThisFrame.avgMinFrameTemp = avgMins;
  avgsThisFrame.avgMaxFrameTemp = avgMaxs;
  avgsThisFrame.avgTireTemp = 0.0;
  avgsThisFrame.avgOuterTireTemp = 0.0;
  avgsThisFrame.avgMiddleTireTemp = 0.0;
  avgsThisFrame.avgInnerTireTemp = 0.0;
  avgsThisFrame.avgOuterAmbientTemp = 0.0;
  avgsThisFrame.avgInnerAmbientTemp = 0.0;

#ifdef FIS_AUTOZOOM
  calculateSlope(measurement_slope);
  getMinMaxSlopePosition();
  validAutozoomFrame = checkAutozoomValidityAndSetAvgTemps();
  if (validAutozoomFrame) {
    float leftStepSize = abs((outerTireEdgePositionThisFrameViaSlopeMax-outerTireEdgePositionSmoothed)/4);
    float rightStepSize = abs((innerTireEdgePositionThisFrameViaSlopeMin-innerTireEdgePositionSmoothed)/4);
    if (outerTireEdgePositionSmoothed < outerTireEdgePositionThisFrameViaSlopeMax) outerTireEdgePositionSmoothed += leftStepSize;
    else outerTireEdgePositionSmoothed -= leftStepSize;
    if (innerTireEdgePositionSmoothed < innerTireEdgePositionThisFrameViaSlopeMin) innerTireEdgePositionSmoothed += rightStepSize;
    else innerTireEdgePositionSmoothed -= rightStepSize;
    if (innerTireEdgePositionSmoothed < 0) outerTireEdgePositionSmoothed = 0;
    if (outerTireEdgePositionSmoothed < 0) innerTireEdgePositionSmoothed = 0;
    if (outerTireEdgePositionSmoothed > FIS_X) outerTireEdgePositionSmoothed = FIS_X;
    if (innerTireEdgePositionSmoothed > FIS_X) innerTireEdgePositionSmoothed = FIS_X;
  }
#else
    avgsThisFrame.avgFrameTemp = getAverage(measurement, FIS_X);
#endif

  // update running averages
  runningAvgOutlierRate += (((float)totalOutliersThisFrame / ((float)FIS_X * (float)EFFECTIVE_ROWS)) - runningAvgOutlierRate) / totalFrameCount;
  runningAvgZoomedFramesRate += ((validAutozoomFrame ? 1 : 0) - runningAvgZoomedFramesRate) / totalFrameCount;
  movingAvgFrameTmp = (0.2 * avgsThisFrame.avgFrameTemp) + (0.8 * movingAvgFrameTmp); // exponential moving average
  movingAvgStdDevFrameTmp = (0.2 * (avgsThisFrame.stdDevFrameTemp < MIN_TMP_STDDEV ? MIN_TMP_STDDEV : avgsThisFrame.stdDevFrameTemp)) + (0.8 * movingAvgStdDevFrameTmp); // exponential moving average retaining a minimum standard deviation of 20 degrees Celsius
  if (totalOutliersThisFrame == 0) { // only if we have an outlier-free frame; exponential moving average 
    movingAvgRowDeltaTmp = (0.2 * (avgsThisFrame.avgMaxFrameTemp - avgsThisFrame.avgMinFrameTemp)) + (0.8 * movingAvgRowDeltaTmp); // exponential moving average
    if (movingAvgRowDeltaTmp > maxRowDeltaTmp) maxRowDeltaTmp = movingAvgRowDeltaTmp; // if we have a new contender for highest (filtered) delta temperature
  }
  
  interpolate((int)outerTireEdgePositionSmoothed, (int)innerTireEdgePositionSmoothed, measurement_16);
}

int16_t TireTreadTemperature::calculateColumnTemperature(int16_t column_content[], uint8_t size) {
#if COLUMN_AGGREGATE == COLUMN_AGGREGATE_MAX
  return getMaximum(column_content, size);
#elif COLUMN_AGGREGATE == COLUMN_AGGREGATE_AVG
  return (int16_t)getAverage(column_content, size);
#elif COLUMN_AGGREGATE == COLUMN_AGGREGATE_AVG_MINUS_OUTLIERS
  totalOutliersThisFrame += removeOutliersChauvenet(column_content, size);
  return (int16_t)getAverage(column_content, size);
#endif
}

void TireTreadTemperature::interpolate(uint8_t startColumn, uint8_t endColumn, int16_t result[]) {
  float stepSize = (endColumn-startColumn)/16.0;
  int16_t x[FIS_X];
  
  for (uint8_t i=0; i<FIS_X; i++) x[i]=i; // Initialize the X axis of an array {0, 1, 2 ... 30, 31}
  Spline linearSpline(x, measurement, FIS_X, 1);
  for (uint8_t i=0; i<16; i++) result[i] = linearSpline.value(startColumn+i*stepSize);
}

void TireTreadTemperature::calculateSlope(int16_t result[]) {
  for (uint8_t i=0; i<FIS_X-1; i++) result[i] = measurement[i+1]-measurement[i];
}

void TireTreadTemperature::getMinMaxSlopePosition() {
  int16_t minSlopeValue = 0;
  int16_t maxSlopeValue = 0;
  for (uint8_t i=0; i<FIS_X-1; i++) {
    if (measurement_slope[i] > maxSlopeValue ) {
      maxSlopeValue = measurement_slope[i];
      outerTireEdgePositionThisFrameViaSlopeMax = i+1; // we want the first pixel on the tire; make up for the shift between measurement_slope[] and measurement[]
    }
    if (measurement_slope[i] < minSlopeValue ) {
      minSlopeValue = measurement_slope[i];
      innerTireEdgePositionThisFrameViaSlopeMin = i;
    }
  }
}

boolean TireTreadTemperature::checkAutozoomValidityAndSetAvgTemps() {
  float avgTireTempThisFrame = 0.0;
  float avgInnerAmbientThisFrame = 0.0;
  float avgOuterAmbientThisFrame = 0.0;
  
  avgsThisFrame.avgFrameTemp = getAverage(measurement, FIS_X);
  avgsThisFrame.stdDevFrameTemp = getStdDev(measurement, FIS_X);

  if (measurement_slope[innerTireEdgePositionThisFrameViaSlopeMin] > -TMP_TRIGGER_DELTA_AMBIENT_TIRE) return false;
  if (measurement_slope[outerTireEdgePositionThisFrameViaSlopeMax-1] < TMP_TRIGGER_DELTA_AMBIENT_TIRE) return false;
  if (innerTireEdgePositionThisFrameViaSlopeMin < outerTireEdgePositionThisFrameViaSlopeMax) return false; // Inner or outer edge of tire out of camera view
  if ((innerTireEdgePositionThisFrameViaSlopeMin-outerTireEdgePositionThisFrameViaSlopeMax+1) < AUTOZOOM_MINIMUM_TIRE_WIDTH) return false; // Too thin tire
  
  for (uint8_t i=0; i<FIS_X; i++) {
    if (i < outerTireEdgePositionThisFrameViaSlopeMax) {
      avgOuterAmbientThisFrame += measurement[i];
    } else if (i > innerTireEdgePositionThisFrameViaSlopeMin) {
      avgInnerAmbientThisFrame += measurement[i];
    } else {
      avgTireTempThisFrame += measurement[i];
    }
  }

  uint8_t tireWidthThisFrame = (innerTireEdgePositionThisFrameViaSlopeMin-outerTireEdgePositionThisFrameViaSlopeMax+1);
  avgTireTempThisFrame = avgTireTempThisFrame / tireWidthThisFrame;
  avgInnerAmbientThisFrame = avgInnerAmbientThisFrame / outerTireEdgePositionThisFrameViaSlopeMax;
  avgOuterAmbientThisFrame = avgOuterAmbientThisFrame / (FIS_X-innerTireEdgePositionThisFrameViaSlopeMin-1);
  if ((avgTireTempThisFrame - avgInnerAmbientThisFrame < TMP_AVG_DELTA_AMBIENT_TIRE) && (innerTireEdgePositionThisFrameViaSlopeMin < (FIS_X-7))) { // if the tire edge is closing in on the sensor FOV, we cannot reasonable determine the ambient temperature any longer => we accept the slope position in a small window near the FOV edge, even if the ambient delta threshold does not hold
    return false; // Tire is not significantly hotter than inner ambient
  }
  if ((avgTireTempThisFrame - avgOuterAmbientThisFrame < TMP_AVG_DELTA_AMBIENT_TIRE) && (outerTireEdgePositionThisFrameViaSlopeMax > 6)) {
    return false; // Tire is not significantly hotter than outer ambient
  }
  avgsThisFrame.avgTireTemp = avgTireTempThisFrame;
  
  float avgOuterTireTempThisFrame = 0.0;
  float avgMiddleTireTempThisFrame = 0.0;
  float avgInnerTireTempThisFrame = 0.0;
  for (uint8_t j=outerTireEdgePositionThisFrameViaSlopeMax; j<=innerTireEdgePositionThisFrameViaSlopeMin; j++) {
    if (j <= (outerTireEdgePositionThisFrameViaSlopeMax+floor(tireWidthThisFrame/3))) {
      avgOuterTireTempThisFrame += measurement[j];
    } else if (j >= (innerTireEdgePositionThisFrameViaSlopeMin-floor(tireWidthThisFrame/3)+1)) {
      avgInnerTireTempThisFrame += measurement[j];
    } else {
      avgMiddleTireTempThisFrame += measurement[j];
    }
  }

  avgsThisFrame.avgOuterTireTemp = avgOuterTireTempThisFrame / floor(tireWidthThisFrame/3);
  avgsThisFrame.avgMiddleTireTemp = avgMiddleTireTempThisFrame / (tireWidthThisFrame-(2*floor(tireWidthThisFrame/3)));
  avgsThisFrame.avgInnerTireTemp = avgInnerTireTempThisFrame / floor(tireWidthThisFrame/3);
  avgsThisFrame.avgOuterAmbientTemp = avgOuterAmbientThisFrame;
  avgsThisFrame.avgInnerAmbientTemp = avgInnerAmbientThisFrame;
  return true;
}

// determine outliers according to Chauvenet's criterion, replace outlier values with ABS_ZERO and return the number of outliers removed
uint16_t TireTreadTemperature::removeOutliersChauvenet(int16_t *arr, int size) {
  int outlierCount = 0;
  const float outlierCriterion = 0.50;
  float significanceLevel = outlierCriterion / size;
  int16_t valueClosestToMean = 0;
  float mean = movingAvgFrameTmp;
  float stdDev = movingAvgStdDevFrameTmp;

  for (uint8_t i=0; i < size; i++) {
    if (abs(arr[i]-mean) < abs(valueClosestToMean-mean)) valueClosestToMean = arr[i];
    
    float prob = cumulativeProbability((float)arr[i], mean, stdDev);
    
    if ( prob < significanceLevel || prob > (1-significanceLevel) || arr[i] == ABS_ZERO || arr[i] == 0 || arr[i] == ((-1 + TEMPOFFSET) * 10 * TEMPSCALING) ) {
      // Serial.printf("OUTLIER DETECTED IN FRAME %.0f: Column temps: ", totalFrameCount);
      // for (uint8_t u=0; u < size; u++) {
      //   Serial.print(arr[u]);
      //   Serial.print(", ");
      // }
      // Serial.print("==> ");
      // Serial.printf("mean: %.1f, ", mean);
      if (outlierCount < (size-1)) {
        // Serial.printf("Outlier value to be removed: %i (probability %.1f%%); ", arr[i], prob*100);
        arr[i] = ABS_ZERO;
        outlierCount++;
      } else {
        // if we are about to remove the last array item, then we retain the one value closest to the mean
        // Serial.printf("Outlier value to be removed: %i (probability %.1f%%); Last column value set to: %i; ", arr[i], prob*100, valueClosestToMean);
        arr[i] = valueClosestToMean;
      }
    }
  }
  // if (outlierCount > 0) Serial.print("\n");
  return outlierCount;
}

int16_t TireTreadTemperature::getMaximum(int16_t arr[], int size) {
  int16_t max_value = arr[0];
  for (uint8_t i=0; i < size; i++) {
    if (arr[i] > ABS_ZERO && arr[i] > max_value) max_value = arr[i]; // ignore ABS_ZERO = outlier
  }
  return max_value;
}

int16_t TireTreadTemperature::getMinimum(int16_t arr[], int size) {
  int16_t min_value = arr[0];
  for (uint8_t i=0; i < size; i++) {
    if (arr[i] > ABS_ZERO && arr[i] < min_value) min_value = arr[i]; // ignore ABS_ZERO = outlier
  }
  return min_value;
}

float TireTreadTemperature::getAverage(int16_t arr[], int size) {
  long total = 0;
  int sizeAfterOutliers = size;
  for (uint8_t i=0; i < size; i++) {
    if (arr[i] > ABS_ZERO) { // ignore ABS_ZERO = outlier
      total += arr[i];
    } else {
      sizeAfterOutliers--;
    }
  }
  float avg = total/(float)sizeAfterOutliers; // float for enhanced precision
  return avg;
}

float TireTreadTemperature::getGeometricMean(int16_t arr[], int size) {
  float m = 1.0;
  long long ex = 0;
  int sizeAfterOutliers = size;

  for (uint8_t i=0; i < size; i++) {
    if (arr[i] > ABS_ZERO) { // ignore ABS_ZERO = outlier
      int exp;
      float f1 = frexp(arr[i], &exp);
      m *= f1;
      ex += exp;
    } else {
      sizeAfterOutliers--;
    }
  }

  float invN = 1.0 / sizeAfterOutliers;
//  float gmMean = powf(std::numeric_limits<float>::radix, ex * invN) * powf(m, invN); // not compatible to nrf52
  float gmMean = scalblnf(1, ex * invN) * powf(m, invN);
  return gmMean;
}

// Welford's algorithm
float TireTreadTemperature::getStdDev(int16_t arr[], int size) {
  float variance = 0.0;
  float M = 0.0;
  float oldM = 0.0;
  float S = 0;
  uint16_t k = 1;
  for (uint8_t i=0; i < size; i++) {
    if (arr[i] > ABS_ZERO) { // ignore ABS_ZERO = outlier
      float x = (float)arr[i];
      oldM = M;
      M = M + (x-M)/k;
      S = S + (x-M)*(x-oldM);
      k++;
    }
  }
  if (k>1) variance = S/(k-1);
  float stdDev = sqrt(variance); // float for enhanced precision
  return stdDev;
}

float TireTreadTemperature::cumulativeProbability(float val, float avg, float stdDev) {
  float thisDev = val - avg;
  if (fabs(thisDev) > (40 * stdDev)) {
    return thisDev < 0 ? 0.0 : 1.0; // if val is more than 40 standard deviations from the mean, 0 or 1 is returned, as in these cases we are close enough
  }
  float prob = 0.5 * (1 + erf(thisDev / (stdDev * M_SQRT2)));
  return prob;
}
