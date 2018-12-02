//Class for a Coulomb Counter instance. 
//enables the reading of voltage, current, charge, and temperature directly from the LTC2944
//enables the reading of power, energy derived from the above data
//update() should be called as often as possible in the loop for accurate integration.
//For every getter called, update() is executed once.

#include <Arduino.h>
#include "LTC2944_Arduino.h"
#include "CoulombCounter.h"

CoulombCounter::CoulombCounter(uint16_t prescalar, float resistor, float vscaler)
	: prescalar(prescalar),
      resistor(resistor),
      vscaler(vscaler),
      pwr_p(0) {
      
      	uint8_t prescalarCode;

      	switch(prescalar) {
      		case 1:
      			prescalarCode = LTC2944_PRESCALAR_M_1;
      			break;
      		case 4:
      			prescalarCode = LTC2944_PRESCALAR_M_4;
      			break;
      		case 16:
      			prescalarCode = LTC2944_PRESCALAR_M_16;
      			break;
      		case 64:
      			prescalarCode = LTC2944_PRESCALAR_M_64;
      			break;
      		case 256:
      			prescalarCode = LTC2944_PRESCALAR_M_256;
      			break;
      		case 1024:
      			prescalarCode = LTC2944_PRESCALAR_M_1024;
      			break;
      		case 4096:
      			prescalarCode = LTC2944_PRESCALAR_M_4096;
      			break;
      		default:
      			prescalarCode = LTC2944_PRESCALAR_M_64;
      			break;
      	}

      	t_p = micros();
      	LTC2944_write_control_code(LTC2944_AUTOMATIC_MODE | prescalarCode);
      	this->reset();
    }

CoulombCounter::CCValues CoulombCounter::update() {//Should be called as often as possible (every loop) for accurate integration
	uint16_t vraw, craw, chraw, traw;

	vraw = LTC2944_read_bit_16(LTC2944_I2C_ADDRESS, LTC2944_VOLTAGE_MSB_REG);
	craw = LTC2944_read_bit_16(LTC2944_I2C_ADDRESS, LTC2944_CURRENT_MSB_REG);
	chraw = LTC2944_read_bit_16(LTC2944_I2C_ADDRESS, LTC2944_ACCUM_CHARGE_MSB_REG);
	traw = LTC2944_read_bit_16(LTC2944_I2C_ADDRESS, LTC2944_TEMPERATURE_MSB_REG);

	ccv.temperature = LTC2944_raw_to_celcius(traw);
	ccv.voltage = vscaler*LTC2944_raw_to_voltage(vraw);
	ccv.current = LTC2944_raw_to_current(craw, resistor);
	ccv.charge = LTC2944_raw_to_mAh(chraw, resistor, prescalar);

	t_c = micros();
	ccv.power = ccv.current * ccv.voltage;;
	ccv.energy += ((ccv.power + pwr_p)/2) * ((double)(t_c - t_p)/MICROSCONSTANT);
	pwr_p = ccv.power;
	t_p = t_c;
	
	return ccv;
}

double CoulombCounter::getVoltage() {
	this->update();
	return ccv.voltage;
}

double CoulombCounter::getCurrent() {
	this->update();
	return ccv.current;
}

double CoulombCounter::getPower() {
	this->update();
	return ccv.power;
}

double CoulombCounter::getCharge() {
	this->update();
	return ccv.charge;
}

float CoulombCounter::getTemperature() {
	this->update();
	return ccv.temperature;
}

double CoulombCounter::getEnergy() {
	this->update();
	return ccv.energy;
}

uint16_t CoulombCounter::getPrescalar() {
	return prescalar;
}

float CoulombCounter::getResistor() {
	return resistor;
}

void CoulombCounter::reset() {
	ccv.energy = 0;
	ccv.power = 0;
	ccv.voltage = 0;
	ccv.charge = 0;
	ccv.current = 0;
	ccv.temperature = 0;
	pwr_p = 0;
	t_p = micros();

	LTC2944_write_bit_16(LTC2944_I2C_ADDRESS, LTC2944_ACCUM_CHARGE_MSB_REG, 0x7FFF);
}