//Author: Leonardo La Rocca
#ifndef Melopero_AMG8833_H_INCLUDED
#define Melopero_AMG8833_H_INCLUDED

#include "Arduino.h"

#define I2C_ADDRESS_A 0x68
#define I2C_ADDRESS_B 0x69

#define MODE_REG_ADDR 0x00
#define RESET_REG_ADDR 0x01
#define FPS_MODE_REGISTER 0x02
#define INTERRUPT_CONTROL_REG_ADDR 0x03
#define STATUS_REG_ADDR 0x04
#define CLEAR_STATUS_REG_ADDR 0x05

#define INT_THR_HIGH_L_REG_ADDRESS 0x08
#define INT_THR_HIGH_H_REG_ADDRESS 0x09
#define INT_THR_LOW_L_REG_ADDRESS 0x0A
#define INT_THR_LOW_H_REG_ADDRESS 0x0B
#define INT_HYSTERESIS_L_REG_ADDRESS 0x0C
#define INT_HYSTERESIS_H_REG_ADDRESS 0x0D

#define INT_TABLE_FIRST_ROW 0x10
#define INT_TABLE_LAST_ROW 0x17

#define THERMISTOR_REGISTER 0x0E

#define FIRST_PIXEL_REGISTER 0x80
#define LAST_PIXEL_REGISTER 0xFE

#define MIN_THRESHOLD_TEMP -2000
#define MAX_THRESHOLD_TEMP 2000

#define INTERRUPT_OCCURRED_MASK 0x02
#define PIXEL_TEMPERATURE_OVERFLOW_MASK 0x04
#define THERMISTOR_TEMPERATURE_OVERFLOW_MASK 0x08

enum class DEVICE_MODE : int {
    NORMAL = 0x00,
    SLEEP = 0x10,
    STAND_BY_60_SEC_INTERMITTANCE = 0x20,
    STAND_BY_10_SEC_INTERMITTANCE = 0x21
};

enum class FPS_MODE : int {
    FPS_10 = 0,
    FPS_1 = 1
};

enum class INT_MODE : int {
    DIFFERENCE = 0,
    ABSOLUTE_VALUE = 1
};

enum class Melopero_AMG8833_ERROR_CODE : int {
    NO_ERROR = 0,
    ERROR_READING = -1,
    ERROR_WRITING = -2,
    ARGUMENT_ERROR = -3
};

class Melopero_AMG8833 {
    //Members
    public:
        uint8_t i2cAddress;
        uint8_t bus;

        float pixelMatrix[8][8];
        float thermistorTemperature;

        bool interruptMatrix[8][8];

        bool interruptTriggered;
        bool pixelTemperatureOverflow;
        bool thermistorTemperatureOverflow;

    //Methods
    public:
        Melopero_AMG8833(uint8_t i2cAddr = I2C_ADDRESS_B);

        int readByte(uint8_t registerAddress);
        int writeByte(uint8_t registerAddress, uint8_t value);

        int setMode(DEVICE_MODE mode = DEVICE_MODE::NORMAL);
        int updateStatus();

        int clearFlags(bool clearInterrupt = true, bool clearPixelTempOF = true, bool clearThermTempOF = true);
        int resetFlags();
        int resetFlagsAndSettings();

        int setFPSMode(FPS_MODE mode);
        int getFPSMode();

        int updatePixelMatrix();
        int updateThermistorTemperature();

        int enableInterrupt(bool enable = true, INT_MODE mode = INT_MODE::ABSOLUTE_VALUE);
        int setInterruptThreshold(float lowThreshold, float highThreshold, float hysteresis = 0);
        int updateInterruptMatrix();

        String getErrorDescription(int errorCode);

    private:
        float parsePixel(uint8_t lsb, uint8_t msb);
        uint16_t to12bitFormat(float temp);
};

#endif // Melopero_AMG8833_H_INCLUDED
