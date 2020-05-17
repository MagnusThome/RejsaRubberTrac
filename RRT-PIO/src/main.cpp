#include <Arduino.h>
#include <Wire.h>
#include <Tasker.h>
#include "Configuration.h"
#include "temp_sensor.h"
#include "dist_sensor.h"
#include "DataSink.h"
#include "Battery.h"
  
TireTreadTemperature* tempSensor;
TireTreadTemperature* tempSensor2;
SuspensionTravel* distSensor;
SuspensionTravel* distSensor2;

config_t configuration;
status_t status = {0,0,0.0,0};
BLEServiceDataSink *thisTrackDayApp;
Tasker tasker;

char wheelPos[] = "  ";  // Wheel position for Tire A

// Function declarations
void updateWheelPos(void);
void printStatus(void);
void blinkOnTempChange(int16_t);
void blinkOnDistChange(uint16_t);
void updateRefreshRate(void);
void updateBleName(void);
void updateBattery(void);


// ----------------------------------------


void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial
  Serial.printf("\nBegin startup. Arduino version: %d\n", ARDUINO);

  readConfiguration(&configuration);
  if (!configuration.initialized || configuration.config_version != CONFIG_V) {
    Serial.printf("Initialized is %d\nSize is %d\n", configuration.initialized, configuration.config_version);
    initConfiguration(&configuration);
  }
  dumpConfiguration(&configuration);

#if BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
  Serial.printf("ESP32 IDF version: %s\n", esp_get_idf_version());
  analogReadResolution(12); //12 bits
  analogSetAttenuation(ADC_11db);  //For all pins
#endif

  if (GPIOLEDDIST > 0) pinMode(GPIOLEDDIST, OUTPUT);
  if (GPIOLEDTEMP > 0) pinMode(GPIOLEDTEMP, OUTPUT);
  pinMode(GPIODISTSENSORXSHUT, OUTPUT);
#if BOARD == BOARD_ESP32_LOLIND32
  pinMode(GPIOUNUSEDA2, INPUT);
  pinMode(GPIOUNUSEDA2, INPUT);
#endif

  updateBattery();
  updateWheelPos();
  updateBleName();
  
  if (configuration.sensor_1.enabled) { 
    #if BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
      Wire.begin(GPIOSDA,GPIOSCL); // initialize I2C Channel 1 w/ I2C pins from config
    #else
      Wire.begin();
    #endif

    if (configuration.sensor_1.dist_type) {
      debug("Starting first distance sensor for %s...\n", wheelPos);
      distSensor = new SuspensionTravel();
      if (distSensor->initialise(&Wire, wheelPos)) {
        debug("First distance sensor for %s present.\n", wheelPos);
      }
      else {
        debug("ERROR: First distance sensor for %s not present.\n", wheelPos);
      }
    }
    debug("Starting first temperature sensor for %s...\n", wheelPos);
    tempSensor = new TireTreadTemperature();
    if (!tempSensor->initialise(&configuration.sensor_1.fis, &status, &Wire)) {
      // perform automatic system reboot to retry temp sensor initialization
      #if BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
        debug("Rebooting the MCU now...\n");
        ESP.restart();
      #elif BOARD == BOARD_NRF52_FEATHER
        debug("Rebooting the MCU now...\n");
        NVIC_SystemReset();
      #endif
    }
  }

  
#if BOARD == BOARD_ESP32_LOLIND32
  if (configuration.sensor_2.enabled) {  
    Wire1.begin(GPIOSDA2,GPIOSCL2); // initialize I2C Channel 2 w/ I2C pins from config

    if (configuration.sensor_2.dist_type) {
      debug("Starting second distance sensor for %s...\n", wheelPos2);
      distSensor2 = new SuspensionTravel();
      if (distSensor2->initialise(&Wire1, wheelPos2)) {
        debug("Second distance sensor for %s present.\n", wheelPos2);
      }
      else {
        debug("ERROR: Second distance sensor for %s not present.\n", wheelPos2);
      }
    }
    debug("Starting second temperature sensor for %s...\n", wheelPos2);
    tempSensor = new TireTreadTemperature();
    if (!tempSensor2->initialise(&configuration.sensor_2.fis, &status, &Wire1)) {
      // perform automatic system reboot to retry temp sensor initialization
      #if BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
        debug("Rebooting the MCU now...\n");
        ESP.restart();
      #endif
    }
  } else {
    // set unused I2C pins to input mode (just to be sure)
    pinMode(GPIOSDA2, INPUT);
    pinMode(GPIOSCL2, INPUT);
  }
#endif

// BLE
  #if BOARD == BOARD_NRF52_FEATHER
    if (!Nrf52BLEDataSink::initializeBLEDevice(bleName)) {
      debug("ERROR initializing BLE Device.\n");
    }
    thisTrackDayApp = new Nrf52TrackDayApp();
  #elif BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
    if (!Esp32BLEDataSink::initializeBLEDevice(configuration, status)) {
      debug("ERROR initializing BLE Device.\n");
    }
    thisTrackDayApp = new Esp32TrackDayApp();
  #endif
    thisTrackDayApp->initializeBLEService();

  #if BOARD == BOARD_NRF52_FEATHER
    if (!Nrf52BLEDataSink::startAdvertising()) {
      debug("ERROR advertising BLE Services.\n");
    }
  #elif BOARD == BOARD_ESP32_FEATHER || BOARD == BOARD_ESP32_LOLIND32
    if (!Esp32BLEDataSink::startAdvertising()) {
      debug("ERROR advertising BLE Services.\n");
    }
  #endif

// Set up periodic functions
#ifdef _DEBUG
  tasker.setInterval(printStatus, SERIAL_UPDATERATE*1000); // Print status every second
#endif
  tasker.setInterval(updateBattery, BATTERY_UPDATERATE*1000); // Update battery status every minute
  tasker.setInterval(updateRefreshRate, 2000);

  debug("Running!\n");
}

void loop() {
  if (configuration.sensor_1.enabled) {
    if (configuration.sensor_1.dist_type) {
      distSensor->measure();
    }
    tempSensor->measure();
  }
  if (configuration.sensor_2.enabled) {
    if (configuration.sensor_2.dist_type) {
      distSensor2->measure();
    }
    tempSensor2->measure();
  }

  if (TrackDayApp::isConfigReceived()) {
    debug("Config received.\n");
    memcpy(&configuration, TrackDayApp::getBleConfig(), sizeof(config_t));
// 2do: encapsulate properly
    tempSensor->initialise(&configuration.sensor_1.fis, &status, &Wire);
    updateBleName();
    Esp32BLEDataSink::setDeviceName(status);
    dumpConfiguration(&configuration);
    writeConfiguration(&configuration);
  }
  thisTrackDayApp->transmit(tempSensor, distSensor, &status, configuration);
  
  blinkOnTempChange(tempSensor->measurement_16[8]/20);    // Use one single temp in the middle of the array
  blinkOnDistChange(distSensor->distance/20);    // value/nn -> Ignore smaller changes to prevent noise triggering blinks

// 2do: integrate tempSensor2 & distSensor2 into BLE transmission

  status.measurementCycles++;

  tasker.loop();
}

// Figure out wheel position coding
void updateWheelPos(void) {
#if FIS_SENSOR2_PRESENT == 1
  if (digitalRead(GPIOLEFT)) {
    // GPIOLEFT  = 0 => axis: both sensors are mounted on the front or rear axle
    wheelPos[1]  = 'L';
    wheelPos2[1] = 'R';

    if (digitalRead(GPIOFRONT)) {
      // GPIOFRONT = 0 => axis 1 (front)
      wheelPos[0]  = 'F';
      wheelPos2[0] = 'F';
      deviceNameSuffix[0] = 'F';
    } else {
      // GPIOFRONT = 1 => axis 2 (rear)
      wheelPos[0]  = 'R';
      wheelPos2[0] = 'R';
      deviceNameSuffix[0] = 'R';
    }
  }
  else {
    // GPIOLEFT  = 1 => axis: axis: both sensors are mounted on the left or right vehicle side
    wheelPos[0]  = 'F';
    wheelPos2[0] = 'R';

    if (digitalRead(GPIOFRONT)) {
      // GPIOFRONT = 0 => axis 1 (left)
      wheelPos[1]  = 'L';
      wheelPos2[1] = 'L';
      deviceNameSuffix[0] = 'L';
    } else {
      // GPIOFRONT = 1 => axis 2 (right)
      wheelPos[1]  = 'R';
      wheelPos2[1] = 'R';
      deviceNameSuffix[0] = 'R';
    }
  }

  // overwrite left/right if we happen to be a motorcycle
  if (!digitalRead(GPIOCAR))  wheelPos[1]  = ' ';
  if (!digitalRead(GPIOCAR))  wheelPos2[1] = ' ';

#else
  uint8_t wheelPosCode = digitalRead(GPIOLEFT) + (digitalRead(GPIOFRONT) << 1) + (digitalRead(GPIOCAR) << 2);
  if (wheelPosCode >= 7) wheelPosCode = DEVICENAMECODE; // set from configuration
  
  sprintf(wheelPos, "%s%s",
          configuration.sensor_1.position.rear ? "R": "F",
          configuration.sensor_1.position.bike ? "": configuration.sensor_1.position.right ? "R" : "L");
#endif
}

void printStatus(void) {
#if DIST_SENSOR != DIST_NONE
  char distSensor_str[6];
  sprintf(distSensor_str, "%imm", distSensor->distance);
#else
  char* distSensor_str = "N/A  ";
#endif
#if DIST_SENSOR2 != DIST_NONE
  char distSensor2_str[6];
  sprintf(distSensor2_str, "%imm", distSensor2->distance);
#endif


  debug("Rate: %.1fHz\tV: %dmV (%d%%) \tWheel: %s\tD: %s\t", (float)status.updateRate, status.mv, status.lipoPercentage, wheelPos, distSensor_str);
  debug("Zoomrate: %.2f%% \tOutliers: %.2f%%\tMaxRowDelta: %.1f\tAvgTemp: %.1f\tAvgStdDev: %.1f\t", tempSensor->runningAvgZoomedFramesRate*100, tempSensor->runningAvgOutlierRate*100, tempSensor->maxRowDeltaTmp/10, tempSensor->movingAvgFrameTmp/10, tempSensor->movingAvgStdDevFrameTmp/10);
  for (uint8_t i=0; i<FIS_X; i++) {
    debug("T: %.1f\t",(float)tempSensor->measurement[i]/10);
  }
  debug("\n");
#if FIS_SENSOR2_PRESENT == 1
  debug("\t\t\t\t\tWheel: %s\tD: %s\t", wheelPos2, distSensor2_str);
  debug("Zoomrate: %.2f%% \tOutliers: %.2f%%\tMaxRowDelta: %.1f\tAvgTemp: %.1f\tAvgStdDev: %.1f\t", tempSensor2->runningAvgZoomedFramesRate*100, tempSensor2->runningAvgOutlierRate*100, tempSensor2->maxRowDeltaTmp/10, tempSensor2->movingAvgFrameTmp/10, tempSensor2->movingAvgStdDevFrameTmp/10);
  for (uint8_t i=0; i<FIS_X; i++) {
    debug("T: %.1f\t",(float)tempSensor2->measurement[i]/10);
  }
  debug("\n");
#endif
}

void updateRefreshRate(void) {
  static long lastUpdate = 0;
  status.updateRate = (float)status.measurementCycles / (millis()-lastUpdate) * 1000;
  lastUpdate = millis();
  status.measurementCycles = 0;
}

void blinkOnDistChange(uint16_t distnew) {
  if (GPIOLEDDIST > 0) {
    static uint16_t distold = 0;

/* DEPRECATED?
    if (!Bluefruit.connected()) {
      digitalWrite(ledDist, HIGH);
      return;
    }
*/
    
    if (distold != distnew) {
      digitalWrite(GPIOLEDDIST, HIGH);
      delay(3);
      digitalWrite(GPIOLEDDIST, LOW);
    }
    distold = distnew;
  }
}

void blinkOnTempChange(int16_t tempnew) {
  if (GPIOLEDTEMP > 0) {
    static int16_t tempold = 0;
    if (tempold != tempnew) {
      digitalWrite(GPIOLEDTEMP, HIGH);
      delay(3);
      digitalWrite(GPIOLEDTEMP, LOW);
    }
    tempold = tempnew;
  }
}

// Figure out final BleName based on the first sensor's position
// 2do: second sensor pod support
void updateBleName(void) {
  sprintf(status.bleName, "%s%s%s",
          configuration.bleNamePrefix,
          configuration.sensor_1.position.rear ? "R": "F",
          configuration.sensor_1.position.bike ? "": configuration.sensor_1.position.right ? "R" : "L");
}

// 2do: move back to Battery.h
void updateBattery(void) {
  status.mv = getVbat() + configuration.voltage_offset;
  status.lipoPercentage = lipoPercent(status.mv);
}
