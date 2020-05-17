#include <Arduino.h>
#include <Preferences.h>
#include "Configuration.h"
#include "config.h"

Preferences prefs;

void readConfiguration(config_t *config) {
    prefs.begin("RRT");
    size_t len = prefs.getBytesLength("configuration");
    char buffer[len];
    if (len != sizeof(config_t)) {
        Serial.printf("Found a different config version, re-initializing configuration.\n");
        initConfiguration(config);
    }
    prefs.getBytes("configuration", buffer, len);
    memcpy(config, buffer, len);
}

void dumpConfiguration(config_t *config) {
    Serial.printf("---\nCurrent Global configuration:\nConfig version: %d\nBoard Type: %d\nVoltage offset: %d\nBLE name: %s\n",
        config->config_version, config->board_type, config->voltage_offset,config->bleNamePrefix);
    Serial.printf("---\nSensor 1 config:\nEnabled: %s\n",
        config->sensor_1.enabled ? "True" : "False");
    if (config->sensor_1.enabled) {
        Serial.printf("FIS Type: %d (%dx%d at %dHz)\n",
            config->sensor_1.fis.type, config->sensor_1.fis.x, config->sensor_1.fis.y, config->sensor_1.fis.refresh_rate);
        Serial.printf("Ignore top rows: %d\nIgnore bottom rows: %d\nScale: %d\nOffset: %1.fC\n",
            config->sensor_1.fis.ignore_top, config->sensor_1.fis.ignore_bottom, config->sensor_1.fis.scale,(float)config->sensor_1.fis.offset/10);
        Serial.printf("Suspension sensor: %d\nPosition: %s %s %s\n",
            config->sensor_1.dist_type, config->sensor_1.position.bike ? "Bike" : "Car", config->sensor_1.position.rear ? "Rear" : "Front", config->sensor_1.position.right? "Right" : "Left");
        Serial.printf("Autozoom: %s\nEmissivity: %d\n",
            config->sensor_1.fis.autozoom ? "Enabled" : "Disabled", config->sensor_1.fis.emissivity);
    }
    Serial.printf("---\nSensor 2 config:\nEnabled: %s\n",
        config->sensor_2.enabled ? "True" : "False");
    if (config->sensor_2.enabled) {
        Serial.printf("FIS Type: %d (%dx%d at %dHz)\n",
            config->sensor_2.fis.type, config->sensor_2.fis.x, config->sensor_2.fis.y, config->sensor_2.fis.refresh_rate);
        Serial.printf("Ignore top rows: %d\nIgnore bottom rows: %d\nScale: %d\nOffset: %1.fC\n",
            config->sensor_2.fis.ignore_top, config->sensor_2.fis.ignore_bottom, config->sensor_2.fis.scale,(float)config->sensor_2.fis.offset/10);
        Serial.printf("Suspension sensor: %d\nPosition: %s %s %s\n",
            config->sensor_2.dist_type, config->sensor_2.position.bike ? "Bike" : "Car", config->sensor_2.position.rear ? "Rear" : "Front", config->sensor_2.position.right? "Right" : "Left");
        Serial.printf("Autozoom: %s\nEmissivity: %d\n",
            config->sensor_2.fis.autozoom ? "Enabled" : "Disabled", config->sensor_2.fis.emissivity);
    }
}

void initConfiguration(config_t *config) {
    Serial.println("Initializing configuration");
    char defaultBleName[16] = "RejsaRubberTrac";
    strncpy(config->bleNamePrefix, defaultBleName, sizeof(defaultBleName) - 1);
    config->initialized = true;
    config->config_version = CONFIG_V;
    config->board_type = BOARD;
    config->voltage_offset = -100;
    config->sensor_1.enabled = true;
    config->sensor_1.fis.type = FIS_SENSOR;
    config->sensor_1.fis.x = FIS_X;
    config->sensor_1.fis.y = FIS_Y;
    config->sensor_1.fis.refresh_rate = FIS_REFRESHRATE;
    config->sensor_1.fis.ignore_top = IGNORE_TOP_ROWS;
    config->sensor_1.fis.ignore_bottom = IGNORE_BOTTOM_ROWS;
    config->sensor_1.fis.autozoom = FIS_AUTOZOOM;
    config->sensor_1.fis.offset = TEMPOFFSET;
    config->sensor_1.fis.scale = TEMPSCALING;
    config->sensor_1.fis.emissivity = EMISSIVITY;
    config->sensor_1.dist_type = DIST_SENSOR;
    config->sensor_1.position.bike = false;
    config->sensor_1.position.right = false;
    config->sensor_1.position.rear = false;
    config->sensor_1.position.mirror = false;
    config->sensor_2.enabled = false;
    config->sensor_2.fis.type = FIS_MLX90640;
    config->sensor_2.fis.x = FIS_X;
    config->sensor_2.fis.y = FIS_Y;
    config->sensor_2.fis.refresh_rate = FIS_REFRESHRATE;
    config->sensor_2.fis.ignore_top = IGNORE_TOP_ROWS;
    config->sensor_2.fis.ignore_bottom = IGNORE_BOTTOM_ROWS;
    config->sensor_2.fis.autozoom = FIS_AUTOZOOM;
    config->sensor_2.fis.offset = TEMPOFFSET;
    config->sensor_2.fis.scale = TEMPSCALING;
    config->sensor_2.fis.emissivity = EMISSIVITY;
    config->sensor_2.dist_type = DIST_SENSOR;
    config->sensor_2.position.bike = false;
    config->sensor_2.position.right = false;
    config->sensor_2.position.rear = false;
    config->sensor_2.position.mirror = false;
    writeConfiguration(config);
}

void writeConfiguration(config_t *config) {
    prefs.putBytes("configuration", config, sizeof(config_t));
}
