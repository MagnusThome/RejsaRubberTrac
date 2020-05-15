#include "DataSink.h"

// definitions for  static member variables
BLEService *BLEServiceDataSink::servicesForAdvertising[4];
uint8_t BLEServiceDataSink::serviceCountForAdvertising;
boolean BLEServiceDataSink::deviceIsAlreadyInitialized;
boolean BLEServiceDataSink::deviceIsAlreadyAdvertising;

#if BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
  BLEServer* Esp32BLEDataSink::thisBLEServer;
  BLEAdvertising* Esp32BLEDataSink::thisBLEAdvertising;
#endif


#if BOARD == BOARD_NRF52_FEATHER
boolean Nrf52BLEDataSink::isConnected() {
  return Bluefruit.connected();
}

boolean Nrf52BLEDataSink::initializeBLEDevice(char bleName[]) {
  if (!deviceIsAlreadyInitialized) {
    uint8_t macaddr[6];

    Serial.printf("Starting BLE device: %s\n", bleName);
    Bluefruit.autoConnLed(false); // DISABLE BLUE BLINK ON CONNECT STATUS
    Bluefruit.begin(2, 0); // allow 2 periphals to connect in parallel
    Bluefruit.getAddr(macaddr);
    sprintf(bleName, "%s%02x%02x%02x", bleName, macaddr[2], macaddr[1], macaddr[0]); // Extend bleName[] with the last 4 octets of the mac address
    Bluefruit.setName(bleName);
    //Serial.print("Starting bluetooth with MAC address ");
    //Serial.printBufferReverse(macaddr, 6, ':');
    //Serial.println();
    deviceIsAlreadyInitialized = true;
    return true;
  }
  else {
    return false;
  }
}

boolean Nrf52BLEDataSink::startAdvertising() {
  if (!deviceIsAlreadyAdvertising) {
    Bluefruit.setTxPower(4);
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Periph.setConnectCallback(connectCallback);
    Bluefruit.Periph.setDisconnectCallback(disconnectCallback);

    switch (BLEServiceDataSink::serviceCountForAdvertising) {
      case 0:  return false; break;
      case 1:  Bluefruit.Advertising.addService(*BLEServiceDataSink::servicesForAdvertising[0]); break;
      case 2:  Bluefruit.Advertising.addService(*BLEServiceDataSink::servicesForAdvertising[0], *BLEServiceDataSink::servicesForAdvertising[1]); break;
      case 3:  Bluefruit.Advertising.addService(*BLEServiceDataSink::servicesForAdvertising[0], *BLEServiceDataSink::servicesForAdvertising[1], *BLEServiceDataSink::servicesForAdvertising[2]); break;
      case 4:  Bluefruit.Advertising.addService(*BLEServiceDataSink::servicesForAdvertising[0], *BLEServiceDataSink::servicesForAdvertising[1], *BLEServiceDataSink::servicesForAdvertising[2], *BLEServiceDataSink::servicesForAdvertising[3]); break;
      default: return false; break;
    }

    Bluefruit.Advertising.addName();
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);
    Bluefruit.Advertising.start(0);
    Serial.println("BLE device start successful, now we are advertising...");
    deviceIsAlreadyAdvertising = true;
    return true;
  }
  else {
    return false;
  }
}

void Nrf52BLEDataSink::connectCallback(uint16_t conn_handle) {
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.println();
  Serial.printf("Connected to %s (handle %u), let's keep advertising for others to come...", central_name, conn_handle);
  Serial.println();

  // restart advertising after connection to a new central
  deviceIsAlreadyAdvertising = false;
  if (!Nrf52BLEDataSink::startAdvertising()) {
    Serial.print("ERROR advertising BLE Services.\n");
  }
}

void Nrf52BLEDataSink::disconnectCallback(uint16_t conn_handle, uint8_t reason) {
  //--------------------------------------------------------------------+
  // HCI STATUS
  //--------------------------------------------------------------------+
  // static lookup_entry_t const _strhci_lookup[] =
  // {
  //   { .key = BLE_HCI_STATUS_CODE_SUCCESS                         , .data = "STATUS_CODE_SUCCESS"                         },
  //   { .key = BLE_HCI_STATUS_CODE_UNKNOWN_BTLE_COMMAND            , .data = "STATUS_CODE_UNKNOWN_BTLE_COMMAND "           },
  //   { .key = BLE_HCI_STATUS_CODE_UNKNOWN_CONNECTION_IDENTIFIER   , .data = "STATUS_CODE_UNKNOWN_CONNECTION_IDENTIFIER"   },
  //   { .key = BLE_HCI_AUTHENTICATION_FAILURE                      , .data = "AUTHENTICATION_FAILURE "                     },
  //   { .key = BLE_HCI_STATUS_CODE_PIN_OR_KEY_MISSING              , .data = "STATUS_CODE_PIN_OR_KEY_MISSING "             },
  //   { .key = BLE_HCI_MEMORY_CAPACITY_EXCEEDED                    , .data = "MEMORY_CAPACITY_EXCEEDED "                   }, // 0x07
  //   { .key = BLE_HCI_CONNECTION_TIMEOUT                          , .data = "CONNECTION_TIMEOUT "                         }, // 0x08
  //   { .key = BLE_HCI_STATUS_CODE_COMMAND_DISALLOWED              , .data = "STATUS_CODE_COMMAND_DISALLOWED "             },
  //   { .key = BLE_HCI_STATUS_CODE_INVALID_BTLE_COMMAND_PARAMETERS , .data = "STATUS_CODE_INVALID_BTLE_COMMAND_PARAMETERS" }, // 0x12=18
  //   { .key = BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION           , .data = "REMOTE_USER_TERMINATED_CONNECTION"           }, // 0x13=19
  //   { .key = BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES , .data = "REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES" }, // 0x14=20
  //   { .key = BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF     , .data = "REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF"     }, // 0x15=21
  //   { .key = BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION            , .data = "LOCAL_HOST_TERMINATED_CONNECTION "           }, // 0x16=22
  //   { .key = BLE_HCI_UNSUPPORTED_REMOTE_FEATURE                  , .data = "UNSUPPORTED_REMOTE_FEATURE"                  },
  //   { .key = BLE_HCI_STATUS_CODE_INVALID_LMP_PARAMETERS          , .data = "STATUS_CODE_INVALID_LMP_PARAMETERS "         },
  //   { .key = BLE_HCI_STATUS_CODE_UNSPECIFIED_ERROR               , .data = "STATUS_CODE_UNSPECIFIED_ERROR"               },
  //   { .key = BLE_HCI_STATUS_CODE_LMP_RESPONSE_TIMEOUT            , .data = "STATUS_CODE_LMP_RESPONSE_TIMEOUT "           },
  //   { .key = BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION , .data = "STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION" },
  //   { .key = BLE_HCI_STATUS_CODE_LMP_PDU_NOT_ALLOWED             , .data = "STATUS_CODE_LMP_PDU_NOT_ALLOWED"             },
  //   { .key = BLE_HCI_INSTANT_PASSED                              , .data = "INSTANT_PASSED "                             },
  //   { .key = BLE_HCI_PAIRING_WITH_UNIT_KEY_UNSUPPORTED           , .data = "PAIRING_WITH_UNIT_KEY_UNSUPPORTED"           },
  //   { .key = BLE_HCI_DIFFERENT_TRANSACTION_COLLISION             , .data = "DIFFERENT_TRANSACTION_COLLISION"             },
  //   { .key = BLE_HCI_PARAMETER_OUT_OF_MANDATORY_RANGE            , .data = "PARAMETER_OUT_OF_MANDATORY_RANGE "           },
  //   { .key = BLE_HCI_CONTROLLER_BUSY                             , .data = "CONTROLLER_BUSY"                             },
  //   { .key = BLE_HCI_CONN_INTERVAL_UNACCEPTABLE                  , .data = "CONN_INTERVAL_UNACCEPTABLE "                 },
  //   { .key = BLE_HCI_DIRECTED_ADVERTISER_TIMEOUT                 , .data = "DIRECTED_ADVERTISER_TIMEOUT"                 },
  //   { .key = BLE_HCI_CONN_TERMINATED_DUE_TO_MIC_FAILURE          , .data = "CONN_TERMINATED_DUE_TO_MIC_FAILURE "         },
  //   { .key = BLE_HCI_CONN_FAILED_TO_BE_ESTABLISHED               , .data = "CONN_FAILED_TO_BE_ESTABLISHED"               }
  // };
  
  Serial.println();
  Serial.printf("Disconnected from handle %u (reason: %u)...", conn_handle, reason);
  Serial.println();

  // restart advertising after connection to a new central
  deviceIsAlreadyAdvertising = false;
  if (!Nrf52BLEDataSink::startAdvertising()) {
    Serial.print("ERROR advertising BLE Services.\n");
  }
}

#elif BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
boolean Esp32BLEDataSink::isConnected() {
  int32_t connectedCount;
  connectedCount = thisBLEServer->getConnectedCount();
  return (connectedCount > 0);
}

boolean Esp32BLEDataSink::initializeBLEDevice(char bleName[]) {
  if (!deviceIsAlreadyInitialized) {
    Serial.printf("Starting BLE device: %s\n", bleName);
    BLEDevice::init(bleName);
    thisBLEServer = BLEDevice::createServer();
    deviceIsAlreadyInitialized = true;
    return true;
  }
  else {
    return false;
  }
}

boolean Esp32BLEDataSink::startAdvertising() {
  if (!deviceIsAlreadyAdvertising) {
    thisBLEAdvertising = BLEDevice::getAdvertising();

    if (BLEServiceDataSink::serviceCountForAdvertising < 1) {
      return false;
    }
    for (uint8_t i=0; i < BLEServiceDataSink::serviceCountForAdvertising; i++) {
      thisBLEAdvertising->addServiceUUID(BLEServiceDataSink::servicesForAdvertising[i]->getUUID());
      Serial.printf("BLE Service UUID for advertising: %s\n", BLEServiceDataSink::servicesForAdvertising[i]->getUUID().toString().c_str());
    }

    thisBLEAdvertising->setScanResponse(false);
    thisBLEAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
    Serial.println("BLE device start successful, now we are advertising...");
    deviceIsAlreadyAdvertising = true;
    return true;
  }
  else {
    return false;
  }
}
#endif

void TrackDayApp::renderPacketTemperature(int16_t measurements[], uint8_t mirrorTire, one_t &FirstPacket, two_t &SecondPacket, thr_t &ThirdPacket) {
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

void TrackDayApp::renderPacketBattery(int vbattery, int percentage, two_t &SecondPacket) {
  SecondPacket.voltage = vbattery;
  SecondPacket.charge = percentage;
}

#if BOARD == BOARD_NRF52_FEATHER
void Nrf52TrackDayApp::initializeBLEService() {
  TrackDayAppService = BLEService(serviceUUID);
  GATTone = BLECharacteristic(0x01);
  GATTtwo = BLECharacteristic(0x02);
  GATTthr = BLECharacteristic(0x03);

  TrackDayAppService.begin();

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

  BLEServiceDataSink::addServiceForAdvertising(&TrackDayAppService);
}

void Nrf52TrackDayApp::transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage) {
  if (isConnected()) {
    renderPacketTemperature(tempMeasurements, mirrorTire, datapackOne, datapackTwo, datapackThr);
    renderPacketBattery(vBattery, lipoPercentage, datapackTwo);
    datapackOne.distance = datapackThr.distance = distance;
    GATTone.notify(&datapackOne, sizeof(datapackOne));
    GATTtwo.notify(&datapackTwo, sizeof(datapackTwo));
    GATTthr.notify(&datapackThr, sizeof(datapackThr));

    measurementCycles++;
  }
}

#elif BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
void Esp32TrackDayApp::initializeBLEService() {
  TrackDayAppService = thisBLEServer->createService(BLEUUID(serviceUUID));
  GATTone = TrackDayAppService->createCharacteristic(BLEUUID((uint16_t)0x0001), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
  GATTtwo = TrackDayAppService->createCharacteristic(BLEUUID((uint16_t)0x0002), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
  GATTthr = TrackDayAppService->createCharacteristic(BLEUUID((uint16_t)0x0003), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
  GATTone->addDescriptor(new BLE2902());
  GATTtwo->addDescriptor(new BLE2902());
  GATTthr->addDescriptor(new BLE2902());
  TrackDayAppService->start();

  BLEServiceDataSink::addServiceForAdvertising(TrackDayAppService);
}

void Esp32TrackDayApp::transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage) {
  if (isConnected()) {
    renderPacketTemperature(tempMeasurements, mirrorTire, datapackOne, datapackTwo, datapackThr);
    renderPacketBattery(vBattery, lipoPercentage, datapackTwo);
    datapackOne.distance = datapackThr.distance = distance;
    GATTone->setValue((uint8_t*)&datapackOne, sizeof(datapackOne));
    GATTtwo->setValue((uint8_t*)&datapackTwo, sizeof(datapackTwo));
    GATTthr->setValue((uint8_t*)&datapackThr, sizeof(datapackThr));
    GATTone->notify();
    GATTtwo->notify();
    GATTthr->notify();

    measurementCycles++;
  }
}
#endif

