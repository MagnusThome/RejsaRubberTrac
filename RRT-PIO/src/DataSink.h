#ifndef DataSink_h
#define DataSink_h

#include <Arduino.h>
#include "protocol.h"
#include "Configuration.h"

#if BOARD == BOARD_NRF52_FEATHER
  #include <bluefruit.h>
  #include <ble_gap.h>
#elif BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
#endif


// Everything that takes RRT data as a data target is a DataSink.
class DataSink
{
  public:
    virtual void transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage) = 0;

    // Optionally call this to throttle down the number of transmits to a minimum interval in milliseconds.
    void setThrottleInterval(uint16_t newThrottleInterval) {
      throttleInterval = newThrottleInterval;
    };

    // do we have a go for this transmit if throttling is enabled (throttleInterval > 0)
    boolean areWeFullThrottle() {
      return (millis() > lastTransmit + throttleInterval);
    };

    // Returns the transmit refresh rate since the last call of getRefreshRate().
    float getRefreshRate() {
      float refreshRate = measurementCycles / (millis()-lastRefreshRateUpdate) * 1000;
      lastRefreshRateUpdate = millis();
      measurementCycles = 0.0;
      return refreshRate;
    };

  protected:
    uint32_t lastTransmit = 0; // needs to be set in transmit()
    uint16_t throttleInterval = 0; // minimum interval between transmits in milliseconds
    uint32_t lastRefreshRateUpdate = 0;
    float measurementCycles = 0.0; // Counts how many measurement cycles were completed since last update of the refresh rate. Needs to be updated in the transmit() function.
};

// One abstract BLE service is the data sink.
class BLEServiceDataSink : public DataSink {
  public:
    virtual void initializeBLEService(void) = 0;

  protected:
    static BLEService *servicesForAdvertising[4]; // maximum 4 services to be advertised by the device
    static uint8_t serviceCountForAdvertising;
    static boolean deviceIsAlreadyInitialized;
    static boolean deviceIsAlreadyAdvertising;

    uint16_t serviceUUID; // The UUID used for advertising this particular BLE service. To be set in the constructor in child classes.

    // All child classes can register their Services for advertising. I.e. startAdvertising() is called only once and will advertise all services in the list.
    static boolean addServiceForAdvertising(BLEService* serviceToAdd) {
      if (serviceCountForAdvertising < 4) {
        servicesForAdvertising[serviceCountForAdvertising] = serviceToAdd;
        serviceCountForAdvertising++;
        return true;
      }
      else
      {
        return false;
      }
    };
};


#if BOARD == BOARD_NRF52_FEATHER
// nRF52-specific initialization logic. Called only once(!), even if multiple BLE services are registered for advertising.
class Nrf52BLEDataSink : public BLEServiceDataSink {
  public:
    static boolean isConnected();
    static boolean initializeBLEDevice(char bleName[]);
    static boolean startAdvertising();
  protected:
    static void connectCallback(uint16_t conn_handle);
    static void disconnectCallback(uint16_t conn_handle, uint8_t reason);
};

#elif BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
// ESP32-specific initialization logic. Called only once(!), even if multiple BLE services are registered for advertising.
class Esp32BLEDataSink : public BLEServiceDataSink {
  public:
    static boolean isConnected();
    static boolean initializeBLEDevice(char bleName[]);
    static boolean startAdvertising();
  protected:
    static BLEServer* thisBLEServer;
    static BLEAdvertising* thisBLEAdvertising;

};
#endif

// All logic and data structures for creating the BLE packets following the protocol for the Track Day mobile Apps supporting RRT (Harry's Laptimer & RaceChrono)
class TrackDayApp {
  public:
    TrackDayApp() { // We initialize the datapacks in the constructor.
      datapackOne.distance = 0;
      datapackOne.protocol = PROTOCOL;
      datapackTwo.protocol = PROTOCOL;
      datapackThr.distance = 0;
      datapackThr.protocol = PROTOCOL;
    }

  protected:
    const uint16_t trackDayAppServiceUUID = (uint16_t)0x1FF7; // The UUID used for advertising the RRT BLE service.

    one_t             datapackOne;
    two_t             datapackTwo;
    thr_t             datapackThr;

    void renderPacketTemperature(int16_t measurements[], uint8_t mirrorTire, one_t &FirstPacket, two_t &SecondPacket, thr_t &ThirdPacket);
    void renderPacketBattery(int vbattery, int percentage, two_t &SecondPacket);
};

#if BOARD == BOARD_NRF52_FEATHER
class Nrf52TrackDayApp : public Nrf52BLEDataSink, TrackDayApp {
  public:
    Nrf52TrackDayApp() {
            serviceUUID = trackDayAppServiceUUID;
    }

    virtual void initializeBLEService();
    virtual void transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage);

  protected:
    BLEService         TrackDayAppService;
    BLECharacteristic  GATTone;
    BLECharacteristic  GATTtwo;
    BLECharacteristic  GATTthr;
};

#elif BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
class Esp32TrackDayApp : public Esp32BLEDataSink, TrackDayApp {
  public:
    Esp32TrackDayApp() {
            serviceUUID = trackDayAppServiceUUID;
    }

    virtual void initializeBLEService(void);
    virtual void transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage);

  protected:
    BLEService*        TrackDayAppService;
    BLECharacteristic* GATTone;
    BLECharacteristic* GATTtwo;
    BLECharacteristic* GATTthr;
};
#endif

#endif
