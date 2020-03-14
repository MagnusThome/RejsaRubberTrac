/*
 * MLX90621.cpp
 *
 *  Created on: 18.11.2013
 *      Author: Max
 *
 *  Adapted by https://github.com/longjos
 *  	Adapted for use with Arduino UNO
 */
#include "MLX90621.h"
#include "Configuration.h"
#include <Wire.h>

void MLX90621::initialise(int refrate, TwoWire *thisI2c) {
	refreshRate = refrate;
//  Wire.begin for initializing I2C needs to be called before MLX initialization
  i2c = thisI2c;
	delay(5);
	readEEPROM();
	writeTrimmingValue();
	setConfiguration();
	preCalculateConstants();
}

void MLX90621::measure(bool calculate_temps) {
	if (checkConfig()) {
		readEEPROM();
		writeTrimmingValue();
		setConfiguration();
	}
	readPTAT();
	readIR();
	if(calculate_temps){
		calculateTA();
		readCPIX();
		calculateTO();
	}

}

float MLX90621::getTemperature(int num) {
	if ((num >= 0) && (num < 64)) {
		return temperatures[num];
	} else {
		return 0;
	}
}

float MLX90621::getAmbient() {
	return Tambient;
}

void MLX90621::setConfiguration() {
	byte Hz_LSB;
	switch (refreshRate) {
	case 0:
		Hz_LSB = 0b00111111;
		break;
	case 1:
		Hz_LSB = 0b00111110;
		break;
	case 2:
		Hz_LSB = 0b00111101;
		break;
	case 4:
		Hz_LSB = 0b00111100;
		break;
	case 8:
		Hz_LSB = 0b00111011;
		break;
	case 16:
		Hz_LSB = 0b00111010;
		break;
	case 32:
		Hz_LSB = 0b00111001;
		break;
	default:
		Hz_LSB = 0b00111110;
	}
	byte defaultConfig_H = 0b01000110;  //kmoto: See data sheet p.11 and 25
	i2c->beginTransmission(0x60);
	i2c->write(0x03);
	i2c->write((byte) Hz_LSB - 0x55);
	i2c->write(Hz_LSB);
	i2c->write(defaultConfig_H - 0x55);
	i2c->write(defaultConfig_H);
	i2c->endTransmission();

	//Read the resolution from the config register
	resolution = (readConfig() & 0x30) >> 4;
}

void MLX90621::readEEPROM() { // Read in blocks of 32 bytes to accomodate Wire library
  for(int j=0;j<256;j+=32) {
    i2c->beginTransmission(0x50);
    i2c->write(j);
    byte rc = i2c->endTransmission(false);
    i2c->requestFrom(0x50, 32);
    for (int i = 0; i < 32; i++) {
      eepromData[j+i] = (uint8_t) i2c->read();
    }
  }
}

void MLX90621::writeTrimmingValue() {
	i2c->beginTransmission(0x60);
	i2c->write(0x04);
	i2c->write((byte) eepromData[OSC_TRIM_VALUE] - 0xAA);
	i2c->write(eepromData[OSC_TRIM_VALUE]);
	i2c->write(0x56);
	i2c->write((byte)0x00);
	i2c->endTransmission();
}

void MLX90621::calculateTA(void) {
	Tambient = ((-k_t1 + sqrt(sq(k_t1) - (4 * k_t2 * (v_th - (float) ptat))))
			/ (2 * k_t2)) + 25.0;
}

void MLX90621::preCalculateConstants() {
	resolution_comp = pow(2.0, (3 - resolution));
	emissivity = unsigned_16(eepromData[CAL_EMIS_H], eepromData[CAL_EMIS_L]) / 32768.0;
	a_common = twos_16(eepromData[CAL_ACOMMON_H], eepromData[CAL_ACOMMON_L]);
	a_i_scale = (int16_t)(eepromData[CAL_AI_SCALE] & 0xF0) >> 4;
	b_i_scale = (int16_t) eepromData[CAL_BI_SCALE] & 0x0F;

	alpha_cp = unsigned_16(eepromData[CAL_alphaCP_H], eepromData[CAL_alphaCP_L]) /
			   (pow(2.0, eepromData[CAL_A0_SCALE]) * resolution_comp);
	a_cp = (float) twos_16(eepromData[CAL_ACP_H], eepromData[CAL_ACP_L]) / resolution_comp;
	b_cp = (float) twos_8(eepromData[CAL_BCP]) / (pow(2.0, (float)b_i_scale) * resolution_comp);
	tgc = (float) twos_8(eepromData[CAL_TGC]) / 32.0;

	k_t1_scale = (int16_t) (eepromData[KT_SCALE] & 0xF0) >> 4;
	k_t2_scale = (int16_t) (eepromData[KT_SCALE] & 0x0F) + 10;
	v_th = (float) twos_16(eepromData[VTH_H], eepromData[VTH_L]);
	v_th = v_th / resolution_comp;
	k_t1 = (float) twos_16(eepromData[KT1_H], eepromData[KT1_L]);
	k_t1 /= (pow(2, k_t1_scale) * resolution_comp);
	k_t2 = (float) twos_16(eepromData[KT2_H], eepromData[KT2_L]);
	k_t2 /= (pow(2, k_t2_scale) * resolution_comp);
}

void MLX90621::calculateTO() {
	float v_cp_off_comp = (float) cpix - (a_cp + b_cp * (Tambient - 25.0));
	tak4 = pow((float) Tambient + 273.15, 4.0);
	minTemp = NULL, maxTemp = NULL;
	for (int i = 0; i < 64; i++) {
		a_ij = ((float) a_common + eepromData[i] * pow(2.0, a_i_scale)) / resolution_comp;
		b_ij = (float) twos_8(eepromData[0x40 + i]) / (pow(2.0, b_i_scale) * resolution_comp);
		v_ir_off_comp = (float) irData[i] - (a_ij + b_ij * (Tambient - 25.0));
		v_ir_tgc_comp = (float) v_ir_off_comp - tgc * v_cp_off_comp;
		float alpha_ij = ((float) unsigned_16(eepromData[CAL_A0_H], eepromData[CAL_A0_L]) / pow(2.0, (float) eepromData[CAL_A0_SCALE]));
		alpha_ij += ((float) eepromData[0x80 + i] / pow(2.0, (float) eepromData[CAL_DELTA_A_SCALE]));
		alpha_ij = alpha_ij / resolution_comp;
		//ksta = (float) twos_16(eepromData[CAL_KSTA_H], eepromData[CAL_KSTA_L]) / pow(2.0, 20.0);
		//alpha_comp = (1 + ksta * (Tambient - 25.0)) * (alpha_ij - tgc * alpha_cp);
		alpha_comp = (alpha_ij - tgc * alpha_cp);  	// For my MLX90621 the ksta calibrations were 0
													// so I can ignore them and save a few cycles
		v_ir_comp = v_ir_tgc_comp / emissivity;
		float temperature = pow((v_ir_comp / alpha_comp) + tak4, 1.0 / 4.0) - 274.15;

		temperatures[i] = temperature;
		if (minTemp == NULL || temperature < minTemp) {
			minTemp = temperature;
		}
		if (maxTemp == NULL || temperature > maxTemp) {
			maxTemp = temperature;
		}
	}
}

float MLX90621::getMinTemp() {
	return minTemp;
}

float MLX90621::getMaxTemp() {
	return maxTemp;
}


void MLX90621::readIR() {
	for (int j = 0; j < 64; j += 16) { // Read in blocks of 32 bytes to overcome Wire buffer limit
		i2c->beginTransmission(0x60);
		i2c->write(0x02);
		i2c->write(j);
		i2c->write(0x01);
		i2c->write(0x20);
		i2c->endTransmission(false);
		i2c->requestFrom(0x60, 32);
		for (int i = 0; i < 16; i++) {
			uint8_t pixelDataLow = (uint8_t) i2c->read();
			uint8_t pixelDataHigh = (uint8_t) i2c->read();
			irData[j + i] = twos_16(pixelDataHigh, pixelDataLow);
		}
	}
}

void MLX90621::readPTAT() {
	i2c->beginTransmission(0x60);
	i2c->write(0x02);
	i2c->write(0x40);
	i2c->write((byte)0x00);
	i2c->write(0x01);
	i2c->endTransmission(false);
	i2c->requestFrom(0x60, 2);
	byte ptatLow = i2c->read();
	byte ptatHigh = i2c->read();
	ptat = (ptatHigh * 256) + ptatLow;

}

void MLX90621::readCPIX() {
	i2c->beginTransmission(0x60);
	i2c->write(0x02);
	i2c->write(0x41);
	i2c->write((byte)0x00);
	i2c->write(0x01);
	i2c->endTransmission(false);
	i2c->requestFrom(0x60, 2);
	byte cpixLow = i2c->read();
	byte cpixHigh = i2c->read();
	cpix = twos_16(cpixHigh, cpixLow);
}

int16_t MLX90621::twos_16(uint8_t highByte, uint8_t lowByte){
	uint16_t combined_word = 256 * highByte + lowByte;
	if (combined_word > 32767)
		return (int16_t) (combined_word - 65536);
	return (int16_t) combined_word;
}

int8_t MLX90621::twos_8(uint8_t byte) {
	if (byte > 127)
		return (int8_t) byte - 256;
	return (int8_t) byte;
}

uint16_t MLX90621::unsigned_16(uint8_t highByte, uint8_t lowByte){
	return (highByte << 8) | lowByte;
}

uint16_t MLX90621::readConfig() {
	i2c->beginTransmission(0x60);
	i2c->write(0x02);
	i2c->write(0x92);
	i2c->write((byte)0x00);
	i2c->write(0x01);
	i2c->endTransmission(false);
	i2c->requestFrom(0x60, 2);
	byte configLow = i2c->read();
	byte configHigh = i2c->read();
	uint16_t config = ((uint16_t) (configHigh << 8) | configLow);
	return config;
}

//Poll the MLX90621 for its current status
//Returns true if the POR/Brown out bit is set
boolean MLX90621::checkConfig() {
	bool check = !((readConfig() & 0x0400) >> 10);
	return check;
}
