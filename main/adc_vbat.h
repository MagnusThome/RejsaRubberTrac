#define NRF52

#define VBAT_PIN A7

#define MILLIVOLTFULLSCALE 3600
#define STEPSFULLSCALE 1024
#define BATRESISTORCOMP 1.403   // Compensation for a resistor voltage divider between battery and ADC input pin


int getVbat() {
  return analogRead(VBAT_PIN) * MILLIVOLTFULLSCALE * BATRESISTORCOMP / STEPSFULLSCALE;
}


uint8_t lipoPercent(float mvolts) {
    if (mvolts >= 4190) { return 100; }
    if (mvolts > 4090)  { return  90 + (mvolts-4090)*10/100; }
    if (mvolts > 3990)  { return  80 + (mvolts-3990)*10/100; }
    if (mvolts > 3890)  { return  60 + (mvolts-3890)*20/100; }
    if (mvolts > 3790)  { return  40 + (mvolts-3790)*20/100; }
    if (mvolts > 3690)  { return  10 + (mvolts-3690)*30/100; }
    if (mvolts > 3590)  { return   0 + (mvolts-3590)*10/100; }
    return 0;
}


// ----------------------------------------
