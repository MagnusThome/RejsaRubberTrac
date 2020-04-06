#include <Arduino.h>
#include "Configuration.h"
#include "Battery.h"

int vBattery = 0;          // Current battery voltage in mV
int lipoPercentage = 0;    // Current battery percentage



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

int getVbat(void) {
  double adcRead=0;
#if BOARD == BOARD_ESP32_FEATHER || BOARD_ESP32_LOLIND32 // Compensation for ESP32's crappy ADC -> https://bitbucket.org/Blackneron/esp32_adc/src/master/
  const double f1 = 1.7111361460487501e+001;
  const double f2 = 4.2319467860421662e+000;
  const double f3 = -1.9077375643188468e-002;
  const double f4 = 5.4338055402459246e-005;
  const double f5 = -8.7712931081088873e-008;
  const double f6 = 8.7526709101221588e-011;
  const double f7 = -5.6536248553232152e-014;
  const double f8 = 2.4073049082147032e-017;
  const double f9 = -6.7106284580950781e-021;
  const double f10 = 1.1781963823253708e-024;
  const double f11 = -1.1818752813719799e-028;
  const double f12 = 5.1642864552256602e-033;

  const int loops = 5;
  const int loopDelay = 1;

  int counter = 1;
  int inputValue = 0;
  double totalInputValue = 0;
  double averageInputValue = 0;
  for (counter = 1; counter <= loops; counter++) {
    inputValue = analogRead(VBAT_PIN);
    totalInputValue += inputValue;
    delay(loopDelay);
  }
  averageInputValue = totalInputValue / loops;
  adcRead = f1 + f2 * pow(averageInputValue, 1) + f3 * pow(averageInputValue, 2) + f4 * pow(averageInputValue, 3) + f5 * pow(averageInputValue, 4) + f6 * pow(averageInputValue, 5) + f7 * pow(averageInputValue, 6) + f8 * pow(averageInputValue, 7) + f9 * pow(averageInputValue, 8) + f10 * pow(averageInputValue, 9) + f11 * pow(averageInputValue, 10) + f12 * pow(averageInputValue, 11);
#elif BOARD == BOARD_NRF52_FEATHER
  adcRead = analogRead(VBAT_PIN);
#endif
  return adcRead * MILLIVOLTFULLSCALE * BATRESISTORCOMP / STEPSFULLSCALE;
}

void updateBattery(void) {
  vBattery = getVbat();
  lipoPercentage = lipoPercent(vBattery);
}
