#include "ble.h"

BLDevice::BLDevice() { // We initialize a couple things in constructor
  datapackOne.distance = 0;
  datapackOne.protocol = PROTOCOL;
  datapackTwo.protocol = PROTOCOL;
  datapackThr.distance = 0;
  datapackThr.protocol = PROTOCOL;
#if BOARD == BOARD_NRF52
  mainService = BLEService(0x00000001000000fd8933990d6f411ff7);
  GATTone = BLECharacteristic(0x01);
  GATTtwo = BLECharacteristic(0x02);
  GATTthr = BLECharacteristic(0x03);
#endif
}

void BLDevice::setupDevice(char bleName[]) {
  uint8_t macaddr[6];

#if BOARD == BOARD_NRF52
  Bluefruit.autoConnLed(false); // DISABLE BLUE BLINK ON CONNECT STATUS
  Bluefruit.begin(); 
  Bluefruit.getAddr(macaddr);
  sprintf(bleName, "%s%02x%02x%02x\0",bleName, macaddr[2], macaddr[1], macaddr[0]); // Extend bleName[] with the last 4 octets of the mac address
  Bluefruit.setName(bleName);
  Serial.print("Starting bluetooth with MAC address ");
//  Serial.printBufferReverse(macaddr, 6, ':');
  Serial.println();
#elif BOARD == BOARD_ESP32
  BLEDevice::init(bleName);
  mainServer = BLEDevice::createServer();
#endif
  Serial.printf("Device name: %s\n", bleName);

  setupMainService();
  startAdvertising();
}

void BLDevice::setupMainService(void) {
#if BOARD == BOARD_NRF52
  mainService.begin();

  GATTone.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);  // Options: CHR_PROPS_BROADCAST, CHR_PROPS_NOTIFY, CHR_PROPS_INDICATE, CHR_PROPS_READ, CHR_PROPS_WRITE_WO_RESP, CHR_PROPS_WRITE
  GATTone.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  GATTone.setFixedLen(20);
  GATTone.begin();

  GATTtwo.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  GATTtwo.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  GATTtwo.setFixedLen(20);
  GATTtwo.begin();

  GATTthr.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  GATTthr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  GATTthr.setFixedLen(20);
  GATTthr.begin();
#elif BOARD == BOARD_ESP32
  mainService = mainServer->createService(BLEUUID((uint16_t)0x1FF7));
  GATTone = mainService->createCharacteristic(BLEUUID((uint16_t)0x0001), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
  GATTtwo = mainService->createCharacteristic(BLEUUID((uint16_t)0x0002), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
  GATTthr = mainService->createCharacteristic(BLEUUID((uint16_t)0x0003), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
  GATTone->addDescriptor(new BLE2902());
  GATTtwo->addDescriptor(new BLE2902());
  GATTthr->addDescriptor(new BLE2902());
  mainService->start();
#endif
}


void BLDevice::startAdvertising(void) {
#if BOARD == BOARD_NRF52
  Bluefruit.setTxPower(4);
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(mainService);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0); 
#elif BOARD == BOARD_ESP32
  mainAdvertising = BLEDevice::getAdvertising();
  mainAdvertising->addServiceUUID(BLEUUID((uint16_t)0x1FF7));
  mainAdvertising->setScanResponse(false);
  mainAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
#endif
}

void BLDevice::renderPacketTemperature(int16_t measurements[], uint8_t mirrorTire, one_t &FirstPacket, two_t &SecondPacket, thr_t &ThirdPacket) {
  for(uint8_t x=0;x<8;x++){
    uint8_t _x = x;
    if (mirrorTire == 1) {
      _x = 7-x;
    }
    FirstPacket.temps[_x]=measurements[x*2];
    SecondPacket.temps[_x]=measurements[x*2 + 1];
    ThirdPacket.temps[_x]=max(FirstPacket.temps[_x], SecondPacket.temps[_x]);
  }
}

void BLDevice::renderPacketBattery(int vbattery, int percentage, two_t &SecondPacket) {
  SecondPacket.voltage = vbattery;
  SecondPacket.charge = percentage;
}

void BLDevice::transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage) {
  renderPacketTemperature(tempMeasurements, mirrorTire, datapackOne, datapackTwo, datapackThr);
  renderPacketBattery(vBattery, lipoPercentage, datapackTwo);
  datapackOne.distance = datapackThr.distance = distance;
#if BOARD == BOARD_NRF52
  GATTone.notify(&datapackOne, sizeof(datapackOne));
  GATTtwo.notify(&datapackTwo, sizeof(datapackTwo));
  GATTthr.notify(&datapackThr, sizeof(datapackThr));
#elif BOARD == BOARD_ESP32
  GATTone->setValue((uint8_t*)&datapackOne, sizeof(datapackOne));
  GATTtwo->setValue((uint8_t*)&datapackTwo, sizeof(datapackTwo));
  GATTthr->setValue((uint8_t*)&datapackThr, sizeof(datapackThr));
  GATTone->notify();
  GATTtwo->notify();
  GATTthr->notify();
#endif 
}

boolean BLDevice::isConnected() {
#if BOARD == BOARD_NRF52
  return Bluefruit.connected();
#elif BOARD == BOARD_ESP32
  int32_t connectedCount;
  connectedCount = mainServer->getConnectedCount();
  return (connectedCount > 0);
#endif
}
