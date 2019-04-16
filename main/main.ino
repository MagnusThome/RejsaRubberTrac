#include <Arduino.h>
#include <Wire.h>
#include "MLX90621.h"
#include "Adafruit_VL53L0X.h"
#include "ble_gatt.h"
#include "adc_vbat.h"

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


#define WHEELPOS 7        // DEFAULT is 7
                          // 7 = "RejsaRubber" + four last bytes from the bluetooth MAC address

                          // 0 = "RejsaRubberFL" + three last bytes from the bluetooth MAC address
                          // 1 = "RejsaRubberFR" + three last bytes from the bluetooth MAC address
                          // 2 = "RejsaRubberRL" + three last bytes from the bluetooth MAC address
                          // 3 = "RejsaRubberRR" + three last bytes from the bluetooth MAC address
                          // 5 = "RejsaRubberF" + one space + three last bytes from the bluetooth MAC address
                          // 6 = "RejsaRubberR" + one space + three last bytes from the bluetooth MAC address
                        
                          // NOTE!!! THIS CAN BE OVERRIDDEN WITH HARDWARE CODING WITH GPIO PINS

                    
#define MIRRORTIRE 0      // 0 = default
                          // 1 = Mirror the tire, making the outside edge temps the inside edge temps

                          // NOTE!!! THIS CAN BE OVERRIDDEN WITH HARDWARE CODING WITH A GPIO PIN
                          
                          

//#define DUMMYDATA       // UNCOMMENT TO ENABLE FAKE RANDOM DATA WITH NO SENSORS NEEDED


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


#define PROTOCOL 0x01
#define TEMPSCALING 1.00  // Default = 1.00
#define TEMPOFFSET 0      // Default = 0

#define GPIODISTSENSORXSHUT 12  // GPIO pin number
#define GPIOCAR    28  // GPIO pin number
#define GPIOFRONT  29  // GPIO pin number
#define GPIOLEFT   13  // GPIO pin number
#define GPIOMIRR   14  // GPIO pin number

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
uint8_t macaddr[6];
char bleName[19];
uint8_t mirrorTire = 0;


Adafruit_VL53L0X distSensor = Adafruit_VL53L0X();
MLX90621 tempSensor; 


// Function declarations
uint8_t InitDistanceSensor(void);
uint8_t getWheelPosCoding(void);
void setBLEname(uint8_t);
void printStatus(void);
void blinkOnTempChange(int16_t);
void blinkOnDistChange(uint16_t);


#ifdef DUMMYDATA
  #include "dummydata.h"
#endif  


// ----------------------------------------

void setup(){
  Serial.begin(115200);
  Serial.println("\nBegin startup");

  Bluefruit.autoConnLed(false); // DISABLE BLUE BLINK ON CONNECT STATUS
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(GPIODISTSENSORXSHUT, OUTPUT);
  pinMode(GPIOLEFT, INPUT_PULLUP);
  pinMode(GPIOFRONT, INPUT_PULLUP);
  pinMode(GPIOCAR, INPUT_PULLUP);
  pinMode(GPIOMIRR, INPUT_PULLUP);
  
  datapackOne.distance = 0;
  datapackOne.protocol = PROTOCOL;
  datapackTwo.protocol = PROTOCOL;

  Serial.println("Starting distance sensor");
  distSensorPresent = InitDistanceSensor();

  Serial.println("Starting temp sensor");
  tempSensor.initialise(16);


  // START UP BLUETOOTH
  Serial.print("Starting bluetooth with MAC address ");
  Bluefruit.begin();
  Bluefruit.Gap.getAddr(macaddr);
  Serial.printBufferReverse(macaddr, 6, ':');
  Serial.println();


  // BLUETOOTH DEVICE NAME
  uint8_t wheelPosCode = getWheelPosCoding();
  if (wheelPosCode < 7) {
    setBLEname(wheelPosCode); // SET FROM GPIO JUMPERS
  }
  else {
    setBLEname(WHEELPOS);     // SET FROM DEFINE IN CODE HEADER
  }
  Serial.print("Device name: ");
  Serial.println(bleName);
  Bluefruit.setName(bleName); 


  // TIRE MIRRORED?
  if ((MIRRORTIRE == 1 || digitalRead(GPIOMIRR) == 0)) {
    mirrorTire = 1;
    Serial.println("Temperature sensor orientation is mirrored");
  }


  // RUN BLUETOOTH GATT
  setupMainService();
  startAdvertising(); 
  Serial.println("Running!");


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
      datapackOne.distance = 0;                                           // SENSOR FAIL
      Serial.println("Reset distance sensor");
      InitDistanceSensor();
    }
    else {
      if (measure.RangeStatus == 4 || measure.RangeMilliMeter > 8190) {   // MEASURE FAIL
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
    uint8_t idx = i;
    if (mirrorTire == 1) {
      idx = 7-i;
    }
    datapackOne.temps[idx] = (int16_t) TEMPOFFSET + TEMPSCALING *((tempSensor.getTemperature(i*8+1)+tempSensor.getTemperature(i*8+2))*5);  // Mean value of the two middle rows of the *four* (4x16) rows total (the first and last rows are ignored)
    datapackTwo.temps[idx] = (int16_t) TEMPOFFSET + TEMPSCALING *((tempSensor.getTemperature(i*8+5)+tempSensor.getTemperature(i*8+6))*5);  
  }


  // - - B A T T E R Y - -
  unsigned long now = millis();
  static unsigned long timer = 60000;  // check every 60 seconds
  if (now - timer >= 60000) {
    timer = now;
    datapackTwo.voltage = getVbat();
    datapackTwo.charge = lipoPercent(datapackTwo.voltage);
  }

  
  if (Bluefruit.connected()) {
    GATTone.notify(&datapackOne, sizeof(datapackOne));
    GATTtwo.notify(&datapackTwo, sizeof(datapackTwo));
  }

  blinkOnTempChange(datapackOne.temps[4]/20);    // Use one single temp in the middle of the array
  blinkOnDistChange(datapackOne.distance/20);    // value/nn -> Ignore smaller changes to prevent noise triggering blinks


  printStatus();

}




// ----------------------------------------

uint8_t InitDistanceSensor(void) {
  digitalWrite(GPIODISTSENSORXSHUT, LOW);
  delay(50);
  digitalWrite(GPIODISTSENSORXSHUT, HIGH);
  delay(50);
  return distSensor.begin(VL53L0X_I2C_ADDR, false); 
}


// ----------------------------------------
uint8_t getWheelPosCoding(void) {
  return digitalRead(GPIOLEFT) + (digitalRead(GPIOFRONT) << 1) + (digitalRead(GPIOCAR) << 2);
}

// ----------------------------------------

void setBLEname(uint8_t wheelPos) {

  if (wheelPos == 0) {
    strncpy(bleName, "RejsaRubberFL", 13);
  }
  else if (wheelPos == 1) {
    strncpy(bleName, "RejsaRubberFR", 13);
  }
  else if (wheelPos == 2) {
    strncpy(bleName, "RejsaRubberRL", 13);
  }
  else if (wheelPos == 3) {
    strncpy(bleName, "RejsaRubberRR", 13);
  }
  else if (wheelPos == 4) {
    strncpy(bleName, "RejsaRubberF ", 13);
  }
  else if (wheelPos == 5) {
    strncpy(bleName, "RejsaRubberF ", 13);
  }
  else if (wheelPos == 6) {
    strncpy(bleName, "RejsaRubberR ", 13);
  }
  else if (wheelPos == 7) {
    strncpy(bleName, "RejsaRubber", 11);
  }
  else {
    strncpy(bleName, "Name Error ", 11);
  }

  uint8_t numAddressBytes;
  if (wheelPos < 7 ) {
    numAddressBytes = 3;
  }
  else {
    numAddressBytes = 4;
  }
  for(uint8_t i=0; i<numAddressBytes; i++) {
    uint8_t a = sizeof(bleName)-(i*2)-2;
    uint8_t b = sizeof(bleName)-(i*2)-1;
    bleName[a] = (macaddr[i] >> 4);  
    if (bleName[a] > 0x9) bleName[a] += 55; else bleName[a] += 48;
    bleName[b] = (macaddr[i] & 0xf); 
    if (bleName[b] > 0x9) bleName[b] += 55; else bleName[b] += 48;
  }
  bleName[sizeof(bleName)] = '\0';

}
  

// ----------------------------------------

void printStatus(void) {

  static unsigned long then;
  unsigned long now = millis();
  Serial.print(1000/(float)(now - then),1); // Print loop speed in Hz
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

  if (!Bluefruit.connected()) {
    digitalWrite(LED_RED, HIGH);
    return;
  }
  
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
