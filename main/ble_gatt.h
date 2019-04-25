#include <bluefruit.h>

BLEService  mainService   = BLEService        (0x00000001000000fd8933990d6f411ff7);
BLECharacteristic GATTone = BLECharacteristic (0x01);
BLECharacteristic GATTtwo = BLECharacteristic (0x02);
BLECharacteristic GATTthr = BLECharacteristic (0x03);



void setupMainService(void) {
  
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
}


void startAdvertising(void) {
  
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
