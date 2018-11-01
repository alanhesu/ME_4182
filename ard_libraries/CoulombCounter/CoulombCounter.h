//CoulombCounter.h
//provides the declarations for the CoulombCounter class.

#ifndef CoulombCounter_H
#define CoulombCounter_H

class CoulombCounter{
	public:
		CoulombCounter(uint8_t prescaler, float resistor);

		void update();//Should be called as often as possible (every loop) for accurate integration

		float getVoltage();

		float getCurrent();

		float getPower();

		float getCharge();

		float getTemperature();

		float getEnergy();

	private:
		int prescaler;//LTC2944 prescaler. Use the macros defined in LTC2944.h
		float resistor;//Resistor value in mOhms
		int8_t ack;//I2C acknowledgement. 0 = acknowledged. 1 = not acknowledged

		float voltage, current, charge, temperature;//LTC2944 data outs

		float pwr_p, pwr_c;//previous and current power measurements
		unsigned long t_p, t_c;//time stamps of previous and current power measurements
		float energy;//Not a direct output. Calculated using trapezoidal integration of power
};



#endif
