#include "Configuration.h"
#if BOARD == BOARD_NRF52_FEATHER
  #include <bluefruit.h>
#elif BOARD == BOARD_ESP32_FEATHER || BOARD_ESP32_LOLIND32
  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
#endif
#include <Arduino.h>
#include "protocol.h"

class BLDevice {
private:
#if BOARD == BOARD_NRF52_FEATHER
  BLEService         mainService;
  BLECharacteristic  GATTone;
  BLECharacteristic  GATTtwo;
  BLECharacteristic  GATTthr;
#elif BOARD == BOARD_ESP32_FEATHER || BOARD_ESP32_LOLIND32
  BLEService*        mainService;
  BLEServer*         mainServer;
  BLEAdvertising*    mainAdvertising;
  BLECharacteristic* GATTone;
  BLECharacteristic* GATTtwo;
  BLECharacteristic* GATTthr;
#endif
  one_t             datapackOne;
  two_t             datapackTwo;
  thr_t             datapackThr;
  void setupMainService(void);
  void startAdvertising(void);
  void renderPacketTemperature(int16_t measurements[], uint8_t mirrorTire, one_t &FirstPacket, two_t &SecondPacket, thr_t &ThirdPacket);
  void renderPacketBattery(int vbattery, int percentage, two_t &SecondPacket);
  void sendPackets(one_t &FirstPacket, two_t &SecondPacket, thr_t &ThirdPacket);
public:
  boolean isConnected();
  void setupDevice(char bleName[]);
  void transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage);
  BLDevice();
};
