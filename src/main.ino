#include <Arduino.h>
#include <Wire.h>
#include "MLX90621.h"
#include "Adafruit_VL53L0X.h"
#include "ble_gatt.h"
#include "adc_vbat.h"


#define DUMMYDATA           // ENABLE DEBUGGING WITH FAKE DATA


#define PROTOCOL 0x01
#define DISTSENSORSLEEP 27   // GPIO pin number

typedef struct {
  uint8_t  protocol;         // currently: 0x01
  uint8_t  unused;
  uint16_t distance;         // millimeters
  int16_t  temps[8];         // all even numbered temp spots (degrees Celsius x 10)
} one_t;


typedef struct {
  uint8_t  protocol;         // currently: 0x01
  uint8_t  charge;           // percent: 0-100
  uint16_t voltage;          // millivolts (normally circa 3500-4200)
  int16_t  temps[8];         // all uneven numbered temp spots (degrees Celsius x 10)
} two_t;


one_t datapackOne;
two_t datapackTwo;
uint8_t distSensorPresent;

Adafruit_VL53L0X distSensor = Adafruit_VL53L0X();
MLX90621 tempSensor; 



// Function declarations
void printStatus(void);
void blinkOnTempChange(int16_t);
void blinkOnDistChange(uint16_t);
uint8_t InitDistanceSensor(void);


#ifdef DUMMYDATA
  #include "dummydata.h"
#endif  

  

// ----------------------------------------

void setup(){
  Serial.begin(115200);
  Serial.println("\nStarting");
  Bluefruit.autoConnLed(false);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(DISTSENSORSLEEP, OUTPUT);

  datapackOne.distance = 0;
  datapackOne.protocol = PROTOCOL;
  datapackTwo.protocol = PROTOCOL;

  Serial.println("Init distance sensor");
  distSensorPresent = InitDistanceSensor();

  Serial.println("Init temp sensor");
  tempSensor.initialise(16);

  Serial.println("Starting bluetooth");
  Bluefruit.begin();
  Bluefruit.setName("RejsaRubberTrac"); 
  setupMainService();
  startAdvertising(); 
  Serial.println("Running");

#ifdef DUMMYDATA
  dummyloop();
#endif  

}


// ----------------------------------------

void loop() {  
  

  // - - D I S T A N C E - -
  if (distSensorPresent) {
    VL53L0X_RangingMeasurementData_t measure;
    if (distSensor.rangingTest(&measure, false) != VL53L0X_ERROR_NONE) {
      datapackOne.distance = 0;                                           // sensor fail
      Serial.println("Reset distance sensor");
      InitDistanceSensor();
    }
    else {
      if (measure.RangeStatus == 4 || measure.RangeMilliMeter > 8190) {   // measure fail
        datapackOne.distance = 0;
      }
      else {
        datapackOne.distance = measure.RangeMilliMeter;
      }
    }
  }

  
  // - - T E M P S - -
  tempSensor.measure(true); 
  for(uint8_t i=0;i<8;i++){
    datapackOne.temps[i] = (int16_t) ((tempSensor.getTemperature(i*8+1)+tempSensor.getTemperature(i*8+2))*5);  // Mean value of the two middle rows of the *four* (4x16) rows total (the first and last rows are ignored)
    datapackTwo.temps[i] = (int16_t) ((tempSensor.getTemperature(i*8+5)+tempSensor.getTemperature(i*8+6))*5);  
  }


  // - - B A T T E R Y - -
  unsigned long now = millis();
  static unsigned long timer = 60000;  // check every 60 seconds
  if (now - timer >= 60000) {
    timer = now;
    datapackTwo.voltage = getVbat();
    datapackTwo.charge = lipoPercent(datapackTwo.voltage);
  }

  
  if ( Bluefruit.connected() ) {
    GATTone.notify(&datapackOne, sizeof(datapackOne));
    GATTtwo.notify(&datapackTwo, sizeof(datapackTwo));
  }


  blinkOnTempChange(datapackOne.temps[4]/10);    // Use one single temp in the middle of the array
  blinkOnDistChange(datapackOne.distance/20);   // value/nn -> Ignore smaller changes to prevent noise triggering blinks

  printStatus();

}




// ----------------------------------------

uint8_t InitDistanceSensor(void) {
  digitalWrite(DISTSENSORSLEEP, LOW);
  delay(50);
  digitalWrite(DISTSENSORSLEEP, HIGH);
  delay(50);
  return distSensor.begin(VL53L0X_I2C_ADDR, false); 
}


// ----------------------------------------

void printStatus(void) {

  static unsigned long then;
  unsigned long now = millis();
  Serial.print(1000/(float)(now - then),1); // Loop speed in Hz
  Serial.print("Hz\t");
  then = now;

  Serial.print(datapackTwo.voltage);
  Serial.print("mV\t");
  Serial.print(datapackTwo.charge);
  Serial.print("%\t");
  Serial.print(datapackOne.distance);
  Serial.print("mm\t");
  for (uint8_t i=0; i<8; i++) {
    Serial.print(datapackOne.temps[i]);
    Serial.print("\t");
    Serial.print(datapackTwo.temps[i]);
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

void blinkOnTempChange(int16_t tempnew) {
  static int16_t tempold = 0;
  if (tempold != tempnew) {
    digitalWrite(LED_BLUE, HIGH);
    delay(3);
    digitalWrite(LED_BLUE, LOW);
  }
  tempold = tempnew;
}


// ----------------------------------------
