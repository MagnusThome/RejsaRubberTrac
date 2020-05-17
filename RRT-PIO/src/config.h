#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define CONFIG_V 2

typedef struct { // Describes a position on the vehicle (car/bike, left/right, front/rear and camera orientation)
    boolean bike;
    boolean right;
    boolean rear;
    boolean mirror;
} tirepos_t;

typedef struct { // This struct describes a camera sensor in the device
    uint8_t  type;
    uint8_t  x;
    uint8_t  y;
    uint8_t  refresh_rate;
    uint8_t  ignore_top;
    uint8_t  ignore_bottom;
    uint8_t  autozoom;
    uint8_t  offset;
    uint16_t scale;
    uint8_t  emissivity;
} fis_t;

typedef struct { // Sensor is the combination of camera + optional distance sensor at the same corner of the vehicle
    boolean   enabled;
    tirepos_t position;
    fis_t     fis;
    uint8_t   dist_type;
} sensor_t;

typedef struct { // Global config structure
    boolean   initialized;
    uint8_t   config_version;
    uint8_t   board_type;
    int16_t   voltage_offset; // To compensate ESP32's ADC
    sensor_t  sensor_1;
    sensor_t  sensor_2;
    char      bleNamePrefix[32]; // Prefix of BLE name for easy identification
} config_t;

typedef struct {
    boolean  lock;
    uint16_t innerEdge;
    uint16_t outerEdge;
    uint8_t  autozoomFailReason;
} autozoom_stat_t;

typedef struct {
    autozoom_stat_t autozoom_stat;
    int16_t ambient_t;
} sensor_stat_t;

typedef struct {
  uint16_t mv;               // Current battery voltage in mV
  uint8_t  lipoPercentage;    // Current battery percentage
  float    updateRate;
  uint16_t measurementCycles;
  sensor_stat_t sensor_stat_1;
  sensor_stat_t sensor_stat_2;
  char     bleName[48];
} status_t;

void readConfiguration(config_t *config);
void initConfiguration(config_t *config);
void dumpConfiguration(config_t *config);
void writeConfiguration(config_t *config);

#endif // CONFIG_H
