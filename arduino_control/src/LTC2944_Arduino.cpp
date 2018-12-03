#include <Arduino.h>
#include "I2C.h"
#include "LTC2944_Arduino.h"


void LTC2944_initialize() {
	I2c.begin();
	I2c.setSpeed(0);
	I2c.pullup(0);
}

byte LTC2944_read_bit_8(uint8_t i2caddr, uint8_t regaddr)  {

	I2c.read(i2caddr, regaddr, (uint8_t)1);
	return(I2c.receive());
}

word LTC2944_read_bit_16(uint8_t i2caddr, uint8_t regaddr) {
	byte msg, lsg;

	I2c.read(i2caddr, regaddr, (uint8_t)2);
	msg = I2c.receive();
	lsg = I2c.receive(); 

	return(word(msg, lsg));
}

void LTC2944_write_bit_8(uint8_t i2caddr, uint8_t regaddr, uint8_t datain) {
	I2c.write(i2caddr, regaddr, datain);
}

void LTC2944_write_bit_16(uint8_t i2caddr, uint8_t regaddr, uint16_t datain) {
	uint8_t data_arr[2];
	data_arr[0] = highByte(datain);
	data_arr[1] = lowByte(datain);
	I2c.write(i2caddr, regaddr, data_arr, 2);
}

void LTC2944_write_control_code(byte control_code) {
	LTC2944_write_bit_8(LTC2944_I2C_ADDRESS, LTC2944_CONTROL_REG, control_code);
}

double LTC2944_raw_to_voltage(word raw_voltage) {
	return ((double)raw_voltage/(65535))*LTC2944_FULLSCALE_VOLTAGE;
}

double LTC2944_raw_to_current(word raw_current, float resistor) {
	return ((((double)raw_current - 32767.0)/(32767.0)) * ((double)(LTC2944_FULLSCALE_CURRENT)/resistor));
}

float LTC2944_raw_to_celcius(word raw_temperature) {
	return (raw_temperature*((float)(LTC2944_FULLSCALE_TEMPERATURE)/65535) - 273.15);
}

double LTC2944_raw_to_mAh(word raw_charge, float resistor, uint16_t prescalar )
{
  return(1000*(double)((double)raw_charge*LTC2944_CHARGE_lsb*prescalar*50E-3)/(resistor*4096));
}