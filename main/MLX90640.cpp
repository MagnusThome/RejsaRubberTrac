#include <Wire.h>
#include "MLX90640.h"
#include "Configuration.h"

boolean MLX90640::initialise(int refrate, TwoWire *thisI2c) {
//  Wire.begin for initializing I2C needs to be called before MLX initialization
  i2c = thisI2c;
  int status;
  
//  MLX90640_I2CFreqSet(800, i2c); //Changing gears, ensure that I2C clock speed set to 1MHz
  MLX90640_I2CFreqSet(1000, i2c); //Changing gears, ensure that I2C clock speed set to 1MHz
 
  static long startTime = millis();
  while (!isConnected()) {
    if (millis() < (startTime+10000)) { // keep trying for 10 seconds
      Serial.printf("Waiting for MLX90640 to connect at I2C address %u...\n", (uint8_t)MLX90640_ADDRESS);
      delay(100);
    } else {
      Serial.printf("ERROR: Failed connecting to MLX90640 at I2C address %u...\n", (uint8_t)MLX90640_ADDRESS);
      return false;
    }
  }
/*
for (int i = 0; i < 5; i++) {
  delay(1000);
  Serial.println("Waiting after isConnected().");
} */
  
  uint16_t eeMLX90640[832];
  status = MLX90640_DumpEE((uint8_t)MLX90640_ADDRESS, eeMLX90640, i2c);
  if (status != 0) {
    Serial.println("ERROR: Failed to load MLX90640 system parameters.");
    return false;
  }
/*
for (int i = 0; i < 5; i++) {
  delay(1000);
  Serial.println("Waiting after MLX90640_DumpEE().");
} */
  
  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (status != 0) {
    Serial.println("ERROR: MLX90640 Parameter extraction failed.");
    return false;
  }

for (int i = 0; i < 5; i++) {
  delay(1000);
  Serial.println("Waiting after MLX90640_ExtractParameters().");
}
  
  byte Hz;
  switch (refrate) {
    case 0:  Hz = 0x00; break;
    case 1:  Hz = 0x01; break;
    case 2:  Hz = 0x02; break;
    case 4:  Hz = 0x03; break;
    case 8:  Hz = 0x04; break;
    case 16: Hz = 0x05; break;
    case 32: Hz = 0x06; break;
    case 64: Hz = 0x07; break;
    default: Hz = 0b00;
  }
  status = MLX90640_SetRefreshRate((uint8_t)MLX90640_ADDRESS, Hz, i2c);
  if (status != 0) {
    Serial.println("ERROR: Setting MLX90640 refresh rate failed.");
    return false;
  }

for (int i = 0; i < 5; i++) {
  delay(1000);
  Serial.println("Waiting after MLX90640_SetRefreshRate().");
}
  
  Serial.printf("MLX90640 initialised correctly at I2C address %u...\n", (uint8_t)MLX90640_ADDRESS);
  return true;
}

boolean MLX90640::isConnected() {
  i2c->beginTransmission((uint8_t)MLX90640_ADDRESS);
  if (i2c->endTransmission() != 0)
    return (false); //Sensor did not ACK
  return (true);
}

void MLX90640::measure(bool) {
  uint16_t mlx90640Frame[834];
  MLX90640_I2CFreqSet(800, i2c); //Changing gears, ensure that I2C clock speed set to 1MHz
  int _stat = MLX90640_GetFrameData((uint8_t)MLX90640_ADDRESS, mlx90640Frame, i2c);
  if (_stat < 0) Serial.printf("GetFrame Error: %d\n", _stat);
  //Vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
  //int subpage = MLX90640_GetSubPageNumber(mlx90640Frame);
  Tambient = MLX90640_GetTa(mlx90640Frame, &mlx90640);
  float tr = Tambient - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
  float emissivity = 1;
  MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, temperatures);
}

float MLX90640::getTemperature(int num) {
  if ((num >= 0) && (num < 768)) {
    return temperatures[num];
  } else {
    return 0;
  }
}
