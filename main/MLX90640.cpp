#include <Wire.h>
#include "MLX90640.h"
#include "Configuration.h"

void MLX90640::initialise(int refrate, TwoWire *thisI2c) {
//  Wire.begin for initializing I2C needs to be called before MLX initialization
  i2c = thisI2c;
  i2c->setClock(400000);
  
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
  MLX90640_address = 0x33;
  if (isConnected() == false)
  {
    Serial.println("MLX90640 not detected at default I2C address. Please check wiring. Freezing.");
    while (1);
  }

  int status;
  uint16_t eeMLX90640[832];
  status = MLX90640_DumpEE(MLX90640_address, eeMLX90640, i2c);
  if (status != 0) Serial.println("Failed to load system parameters");

  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (status != 0) Serial.println("Parameter extraction failed");
  MLX90640_SetRefreshRate(MLX90640_address, Hz, i2c);

  MLX90640_I2CFreqSet(800, i2c); //Changing gears, ensure that I2C clock speed set to 1MHz
  Serial.println("MLX90640 initialised correctly.");
}

boolean MLX90640::isConnected() {
  i2c->beginTransmission((uint8_t)MLX90640_address);
  if (i2c->endTransmission() != 0)
    return (false); //Sensor did not ACK
  return (true);
}

void MLX90640::measure(bool) {
  uint16_t mlx90640Frame[834];
  MLX90640_I2CFreqSet(800, i2c); //Changing gears, ensure that I2C clock speed set to 1MHz
  int _stat = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame, i2c);
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
