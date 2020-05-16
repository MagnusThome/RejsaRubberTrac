#ifndef DataSink_BluefruitLE_h
#define DataSink_BluefruitLE_h

#include <Arduino.h>
#include "Configuration.h"
#include "DataSink.h"

#if BOARD == BOARD_NRF52_FEATHER
#include <bluefruit.h>


class Nrf52BluefruitLEMQTT : public Nrf52BLEDataSink {
  public:
    Nrf52BluefruitLEMQTT() {
            // serviceUUID = ;
    }

    virtual void initializeBLEService();
    virtual void transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage);

  protected:
//    BLEDfu  bledfu;  // OTA DFU service
//    BLEDis  bledis;  // device information
    BLEUart bleuart; // uart over ble
//    BLEBas  blebas;  // battery
};
#endif


#endif