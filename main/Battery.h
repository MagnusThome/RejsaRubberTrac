#ifndef Battery_h
#define Battery_h

#include <Arduino.h>

extern int vBattery;          // Current battery voltage in mV
extern int lipoPercentage;    // Current battery percentage

uint8_t lipoPercent(float mvolts);
int getVbat(void);
void updateBattery(void);

#endif
