#include "DataSink_BluefruitLE.h"

#if BOARD == BOARD_NRF52_FEATHER
void Nrf52BluefruitLEMQTT::initializeBLEService() {
  // To be consistent OTA DFU should be added first if it exists
//   bledfu.begin();

  // Configure and Start Device Information Service
//   bledis.setManufacturer("Adafruit Industries");
//   bledis.setModel("Bluefruit Feather52");
//   bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();
  BLEServiceDataSink::addServiceForAdvertising(&bleuart);
  setThrottleInterval(2100); // set throttle to 2.1 seconds (Adafruit IO free limit is 30 values per second)
  
  // Start BLE Battery Service
//   blebas.begin();
//   blebas.write(100);
}

void Nrf52BluefruitLEMQTT::transmit(int16_t tempMeasurements[], uint8_t mirrorTire, int16_t distance, int vBattery, int lipoPercentage) {
  if (isConnected() && areWeFullThrottle()) {

    // // Forward data from HW Serial to BLEUART
    // while (Serial.available())
    // {
    //   // Delay to wait for enough input, since we have a limited transmission buffer
    //   delay(2);

    //   uint8_t buf[64];
    //   int count = Serial.readBytes(buf, sizeof(buf));
    //   bleuart.write( buf, count );
    // }
    
    //bleuart.printf("V: %dmV (%d%%)\tD: %i\n", vBattery, lipoPercentage, distance);

    // bleuart.write("12345678911234567892");

    uint8_t temps[20] = { 10,20,30,40,50,60,70,80,90,10,11,12,13,14,15,16,17,18,19,20 }; // 20 bytes of payload available
    bleuart.write( temps, 20 );

    // " (2<FPZ  "
    // https://onlineutf8tools.com/convert-utf8-to-bytes
    // 20 14 1e 28 32 3c 46 50 5a 20 0b 0c 20 0e 0f 10 11 12 13 14
    byte reImported = (byte)0x20;
    uint8_t reImportedUInt8 = (uint8_t)reImported;
    Serial.println();
    Serial.println(reImportedUInt8);
    Serial.println();
    
    lastTransmit = millis();
    measurementCycles++;
  }
}
#endif
