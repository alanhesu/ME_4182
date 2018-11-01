//Class for a Coulomb Counter instance. 
//enables the reading of voltage, current, charge, and temperature directly from the LTC2944
//enables the reading of power, energy derived from the above data
//update() should be called as often as possible in the loop for accurate integration.
//For every getter called, update() is executed once.

#include <Arduino.h>
#include "LTC2944.h"
#include "CoulombCounter.h"

CoulombCounter::CoulombCounter(uint8_t prescaler, float resistor)
	: prescaler(prescaler),
      resistor(resistor),
      energy(0) {
      
      uint8_t control_code = LTC2944_AUTOMATIC_MODE | prescaler | LTC2944_ALERT_MODE;
      ack = LTC2944_write(LTC2944_I2C_ADDRESS, LTC2944_CONTROL_REG, control_code);
    }

void CoulombCounter::update() {//Should be called as often as possible (every loop) for accurate integration
	uint16_t v_code, c_code, t_code;//raw bitvalues read from the LTC2944
      
	ack = LTC2944_read_16_bits(LTC2944_I2C_ADDRESS, LTC2944_VOLTAGE_MSB_REG, &v_code);//read bit voltage
	ack = LTC2944_read_16_bits(LTC2944_I2C_ADDRESS, LTC2944_CURRENT_MSB_REG, &c_code);//read bit current
	ack = LTC2944_read_16_bits(LTC2944_I2C_ADDRESS, LTC2944_TEMPERATURE_MSB_REG, &t_code);//read bit temp
    
	voltage = LTC2944_code_to_voltage(v_code);//convert bits to voltage (V)
	current = LTC2944_code_to_current(c_code,resistor);//convert bits to current (mA)
	temperature = LTC2944_code_to_celcius_temperature(t_code);//convert bits to celcius (C)
	charge = LTC2944_code_to_mAh(c_code,resistor,prescaler);//convert bits to charge (mAh)
      
	pwr_p = pwr_c;//store old power
	t_p = t_c;//store timestamp of old power

	pwr_c = voltage * current;//calculate power
	t_c = micros();//timestamp of calculation

	energy += (pwr_c - pwr_p)/(2*(t_c - t_p));
}

float CoulombCounter::getVoltage() {
	this->update();
	return voltage;
}
float CoulombCounter::getCurrent() {
	this->update();
	return current;
}
float CoulombCounter::getPower() {
	this->update();
	return pwr_c;
}
float CoulombCounter::getCharge() {
	this->update();
	return charge;
}
float CoulombCounter::getTemperature() {
	this->update();
	return temperature;
}
float CoulombCounter::getEnergy() {
	this->update();
	return energy;
}
