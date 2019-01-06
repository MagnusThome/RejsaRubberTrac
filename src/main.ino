#include <Arduino.h>
#include <Wire.h>
#include "MLX90621.h"
#include "Adafruit_VL53L0X.h"
#include "ble_gatt.h"
#include "adc_vbat.h"

uint16_t distData[1];
uint8_t tempData[16];
uint16_t battData[2];


Adafruit_VL53L0X distSensor = Adafruit_VL53L0X();
MLX90621 tempSensor; 


// Function declarations
void printStatus(void);
void blinkOnDistChange(uint16_t);
void blinkOnTempChange(uint8_t);
  

// ----------------------------------------

void setup(){
  Serial.begin(115200);
  Serial.println("\nStarting");
  Bluefruit.autoConnLed(false);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  
  Serial.println("Init distance sensor");
  if (distSensor.begin(VL53L0X_I2C_ADDR, false));
  Serial.println("Init temp sensor");
  tempSensor.initialise(2);

  Serial.println("Starting bluetooth");
  Bluefruit.begin();
  Bluefruit.setName("RejsaRubberTracker"); 
  setupMainService();
  startAdvertising(); 
  Serial.println("Running");
}


// ----------------------------------------

void loop() {  
  

  delay(60);  // MAIN LOOP TIMING
  unsigned long now = millis();


  // - - D I S T A N C E - -
  VL53L0X_RangingMeasurementData_t measure;
  if (distSensor.rangingTest(&measure, false) != VL53L0X_ERROR_NONE) {
    distSensor.begin();   // Restart sensor on error
    distData[0] = 0;
  }
  else {
    distData[0] = measure.RangeMilliMeter;
    if (measure.RangeStatus == 4) {
      distData[0] = 0;
    }
  }
  if ( Bluefruit.connected() ) {
    distCharacteristics.notify(distData, sizeof(distData));
  }
  blinkOnDistChange(distData[0]/10);   // nn/10 -> Ignore smaller changes, noise would be enough to trigger blinking all the time
  

  
  // - - T E M P S - -
  static unsigned long tTimer = 500;  // 2Hz
  if (now - tTimer >= 500) {
    tTimer = now;
    tempSensor.measure(true); 
    for(uint8_t i=0;i<16;i++){
      tempData[i] = (uint8_t) ((tempSensor.getTemperature(i*4+1)+tempSensor.getTemperature(i*4+2))/2);  // Mean value of the two middle rows of the four rows total (the first and last rows are ignored)
    }
    if ( Bluefruit.connected() ) {
      tempCharacteristics.notify(tempData, sizeof(tempData));
    }
    blinkOnTempChange(tempData[8]); // Use one single temp in the middle of the array
  }



  // - - B A T T E R Y - -
  static unsigned long bTimer = 600000; // 10 minutes
  if (now - bTimer >= 1000) {
    bTimer = now;
    battData[0] = getVbat();
    battData[1] = lipoPercent(battData[0]);
    if ( Bluefruit.connected() ) {
      battCharacteristics.notify(battData, sizeof(battData));
    }
  }

  printStatus();
}




// ----------------------------------------

void printStatus(void) {
  Serial.print(battData[0]);
  Serial.print("mV\t");
  Serial.print(battData[1]);
  Serial.print("%\t");
  Serial.print(distData[0]);
  Serial.print("mm\t");
  for (uint8_t i=0; i<16; i++) {
    Serial.print(tempData[i]);
    Serial.print("\t");
  }
  Serial.println();
}


// ----------------------------------------

void blinkOnDistChange(uint16_t distnew) {
  static uint16_t distold = 0;
  if (distold != distnew) {
    digitalWrite(LED_RED, HIGH);
    delay(3);
    digitalWrite(LED_RED, LOW);
  }
  distold = distnew;
}


// ----------------------------------------

void blinkOnTempChange(uint8_t tempnew) {
  static uint8_t tempold = 0;
  if (tempold != tempnew) {
    digitalWrite(LED_BLUE, HIGH);
    delay(3);
    digitalWrite(LED_BLUE, LOW);
  }
  tempold = tempnew;
}


// ----------------------------------------
