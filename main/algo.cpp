#include "algo.h"
#include <Arduino.h>

uint16_t get_maximum(uint16_t arr[], int size) {
  uint16_t max_value = arr[0];
  for (uint8_t i=0; i < size; i++) {
    if (arr[i] > max_value) max_value = arr[i];
  }
  return max_value;
}

uint16_t get_minimum(uint16_t arr[], int size) {
  uint16_t min_value = arr[0];
  for (uint8_t i=0; i < size; i++) {
    if (arr[i] < min_value) min_value = arr[i];
  }
  return min_value;
}

uint16_t get_average(uint16_t arr[], int size) {
  uint16_t sum_value = 0;
  for (uint16_t i=0; i < size; i++) sum_value += arr[i];
  return (uint16_t)sum_value/size;
}

int16_t distanceFilter(int16_t distanceIn) {
  const uint8_t filterSz = 8;
  static int16_t filterArr[filterSz];
  int16_t distanceOut = 0;
  for (int8_t i=0; i<(filterSz-1); i++) {
    filterArr[i] = filterArr[i+1];
    distanceOut += filterArr[i+1];
  }
  filterArr[filterSz-1] = distanceIn;
  distanceOut += distanceIn;
  return (int16_t) distanceOut/filterSz;
}

uint8_t lipoPercent(float mvolts) {
    if (mvolts >= 4200) { return 100; }
    if (mvolts > 4100)  { return  90 + (mvolts-4100)*10/100; }
    if (mvolts > 4000)  { return  80 + (mvolts-4000)*10/100; }
    if (mvolts > 3900)  { return  70 + (mvolts-3900)*10/100; }
    if (mvolts > 3800)  { return  50 + (mvolts-3800)*20/100; }
    if (mvolts > 3700)  { return  30 + (mvolts-3700)*20/100; }
    if (mvolts > 3600)  { return  20 + (mvolts-3600)*10/100; }
    if (mvolts > 3500)  { return  10 + (mvolts-3500)*10/100; }
    if (mvolts > 3400)  { return   2 + (mvolts-3400)*8/100; }
    if (mvolts > 3300)  { return   1 + (mvolts-3300)*1/100; }
    return 1;
}
