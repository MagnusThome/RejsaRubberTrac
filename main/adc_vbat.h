#define NRF52

#define VBAT_PIN A7

#define MILLIVOLTFULLSCALE 3600
#define STEPSFULLSCALE 1024
#define BATRESISTORCOMP 1.403   // Compensation for a resistor voltage divider between battery and ADC input pin


int getVbat() {
  return analogRead(VBAT_PIN) * MILLIVOLTFULLSCALE * BATRESISTORCOMP / STEPSFULLSCALE;
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


// ----------------------------------------
