#include <bluefruit.h>

BLEService        mainService         = BLEService        (0x00000001000000fd8933990d6f411ff7);
BLECharacteristic distCharacteristics = BLECharacteristic (0x01);
BLECharacteristic tempCharacteristics = BLECharacteristic (0x02);
BLECharacteristic battCharacteristics = BLECharacteristic (0x03);



void setupMainService(void) {
  mainService.begin();

  distCharacteristics.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);  // Options: CHR_PROPS_BROADCAST, CHR_PROPS_NOTIFY, CHR_PROPS_INDICATE, CHR_PROPS_READ, CHR_PROPS_WRITE_WO_RESP, CHR_PROPS_WRITE
  distCharacteristics.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  distCharacteristics.setFixedLen(2);
  distCharacteristics.begin();

  tempCharacteristics.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  tempCharacteristics.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  tempCharacteristics.setFixedLen(16);
  tempCharacteristics.begin();

  battCharacteristics.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  battCharacteristics.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  battCharacteristics.setFixedLen(4);
  battCharacteristics.begin();
}


void startAdvertising(void)
{
  Bluefruit.setTxPower(4);
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(mainService);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 160); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0); 
}


// ----------------------------------------
