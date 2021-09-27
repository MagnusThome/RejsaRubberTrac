//Author: Leonardo La Rocca
#include "Melopero_AMG8833.h"
#include "Wire.h"

Melopero_AMG8833::Melopero_AMG8833(uint8_t i2cAddr){
    this->i2cAddress = i2cAddr;
    Wire.begin();
}

int Melopero_AMG8833::readByte(uint8_t registerAddress){
    Wire.beginTransmission(this->i2cAddress);
    Wire.write(registerAddress);
    if (Wire.endTransmission(false) != 0)
        return (int) Melopero_AMG8833_ERROR_CODE::ERROR_READING;

    Wire.requestFrom(this->i2cAddress, 1);
    if (Wire.available())
        return Wire.read();
    else
        return (int) Melopero_AMG8833_ERROR_CODE::ERROR_READING;
}

int Melopero_AMG8833::writeByte(uint8_t registerAddress, uint8_t value){
    Wire.beginTransmission(this->i2cAddress);
    if (Wire.endTransmission(false) != 0)
        return (int) Melopero_AMG8833_ERROR_CODE::ERROR_WRITING;

    Wire.beginTransmission(this->i2cAddress);
    Wire.write(registerAddress);
    Wire.write(value);
    Wire.endTransmission();
    return (int) Melopero_AMG8833_ERROR_CODE::NO_ERROR;
}

/*Set's the device operating mode.
Note:
    Writing operations in Sleep mode are not permitted. Just the set_mode to NORMAL_MODE is permitted.
    Reading operations in Sleep mode are not permitted.*/
int Melopero_AMG8833::setMode(DEVICE_MODE mode){
    return this->writeByte(MODE_REG_ADDR, (uint8_t) mode);
}

int Melopero_AMG8833::updateStatus(){
    int status = this->readByte(STATUS_REG_ADDR);
    if (status < 0) return (int) status;

    this->interruptTriggered = (status & INTERRUPT_OCCURRED_MASK) > 0? true : false;
    this->pixelTemperatureOverflow = (status & PIXEL_TEMPERATURE_OVERFLOW_MASK) > 0? true : false;
    this->thermistorTemperatureOverflow = (status & THERMISTOR_TEMPERATURE_OVERFLOW_MASK) > 0? true : false;

    return (int) Melopero_AMG8833_ERROR_CODE::NO_ERROR;
}

/*Clears the specified flags.*/
int Melopero_AMG8833::clearFlags(bool clearInterrupt, bool clearPixelTempOF, bool clearThermTempOF){
    uint8_t value = clearThermTempOF << 3 | clearPixelTempOF << 2 | clearInterrupt << 1;
    return this->writeByte(CLEAR_STATUS_REG_ADDR, value);
}

/*Clears the Status Register, Interrupt Flag, and Interrupt Table.*/
int Melopero_AMG8833::resetFlags(){
    return this->writeByte(RESET_REG_ADDR, 0x30);
}

/*Resets flags and returns to initial setting.*/
int Melopero_AMG8833::resetFlagsAndSettings(){
    return this->writeByte(RESET_REG_ADDR, 0x3F);
}

int Melopero_AMG8833::setFPSMode(FPS_MODE mode){
    return this->writeByte(FPS_MODE_REGISTER, (uint8_t) mode);
}

int Melopero_AMG8833::getFPSMode(){
    int value = this->readByte(FPS_MODE_REGISTER);
    if (value < 0) return value;
    return value & 1;
}

int Melopero_AMG8833::enableInterrupt(bool enable, INT_MODE mode){
    uint8_t regValue = ((uint8_t) mode) << 1 | enable;
    return this->writeByte(INTERRUPT_CONTROL_REG_ADDR, regValue);
}

int Melopero_AMG8833::setInterruptThreshold(float lowThreshold, float highThreshold, float hysteresis){
    if (lowThreshold < MIN_THRESHOLD_TEMP || lowThreshold > MAX_THRESHOLD_TEMP ||
        highThreshold < MIN_THRESHOLD_TEMP || highThreshold > MAX_THRESHOLD_TEMP)
        return (int) Melopero_AMG8833_ERROR_CODE::ARGUMENT_ERROR;

    uint16_t lowThrRegFormat = this->to12bitFormat(lowThreshold);
    uint16_t highThrRegFormat = this->to12bitFormat(highThreshold);
    uint16_t hysteresisRegFormat = this->to12bitFormat(hysteresis);

    this->writeByte(INT_THR_HIGH_L_REG_ADDRESS, highThrRegFormat & 0x00FF);
    this->writeByte(INT_THR_HIGH_H_REG_ADDRESS, highThrRegFormat & 0xFF00);
    this->writeByte(INT_THR_LOW_L_REG_ADDRESS, lowThrRegFormat & 0x00FF);
    this->writeByte(INT_THR_LOW_H_REG_ADDRESS, lowThrRegFormat & 0xFF00);
    this->writeByte(INT_HYSTERESIS_L_REG_ADDRESS, hysteresisRegFormat & 0x00FF);
    return this->writeByte(INT_HYSTERESIS_H_REG_ADDRESS, hysteresisRegFormat & 0xFF00);
}

uint16_t Melopero_AMG8833::to12bitFormat(float temp){
    uint16_t value = temp >= 0? (uint16_t) (temp * 4) : (uint16_t) (-temp * 4);
    if (temp < 0){
        value = ~value;
        value += 1;
        value &= 0x0FFF;
        value |= 0x0800;
    }
    return value;
}

int Melopero_AMG8833::updateInterruptMatrix(){
    int currRowAddress = INT_TABLE_FIRST_ROW;
    for (int rowIndex = 0; rowIndex < 8; rowIndex++){
        int rowValue = this->readByte(currRowAddress);
        if (rowValue < 0)
            return rowValue;
        for (int colIndex = 0; colIndex < 8; colIndex++)
            this->interruptMatrix[rowIndex][colIndex] = (rowValue & (1 << colIndex)) > 0;
        currRowAddress++;
    }
    return (int) Melopero_AMG8833_ERROR_CODE::NO_ERROR;
}



int Melopero_AMG8833::updatePixelMatrix(){
    int bufferSize = 0;
    int i = 0;
    int j = 0;

    for (int regAddr = FIRST_PIXEL_REGISTER; regAddr <= LAST_PIXEL_REGISTER; regAddr += 2){
        int lsb = this->readByte(regAddr);
        int msb = this->readByte(regAddr + 1);

        if (msb < 0 || lsb < 0) return (int) Melopero_AMG8833_ERROR_CODE::ERROR_READING;

        pixelMatrix[i][j] = this->parsePixel((uint8_t)lsb, (uint8_t)msb);
        j += 1;
        if (j >= 8) {
            j = 0;
            i++;
        }
    }
    return (int) Melopero_AMG8833_ERROR_CODE::NO_ERROR;
}

float Melopero_AMG8833::parsePixel(uint8_t lsb, uint8_t msb){
    int unified_no_sign = ((msb & 7) << 8) | lsb;
    int value = (msb & 8) == 0 ? 0 : - (1 << 11);
    value += unified_no_sign;
    return ((float) value) / 4.0;
}

int Melopero_AMG8833::updateThermistorTemperature(){
    //retrieve register data
    int lsb = this->readByte(THERMISTOR_REGISTER);
    int msb = this->readByte(THERMISTOR_REGISTER + 1);
    //check everything went right
    if (msb < 0 || lsb < 0) return (int) Melopero_AMG8833_ERROR_CODE::ERROR_READING;

    //parse data
    int sign = (msb & 0x08) > 0 ? -1 : 1;
    int value = ((msb & 0x07) << 4) | ((lsb & 0xF0) >> 4);
    int frac = (lsb & 0x0F);
    float frac_temp = frac == 0 ? 0 : 1.0 / ((float) frac);
    this->thermistorTemperature = sign * (((float) value) + frac_temp);

    return (int) Melopero_AMG8833_ERROR_CODE::NO_ERROR;
}

String Melopero_AMG8833::getErrorDescription(int errorCode){
    if (errorCode >= 0)
        return "No Error";
    if (errorCode == -1)
        return "Error reading from device";
    if (errorCode == -2)
        return "Error writing to device";
    if (errorCode == -3)
        return "Argument error: argument value is out of valid range";
}
