//CoulombCounter.h
//provides the declarations for the CoulombCounter class.

#ifndef CoulombCounter_H
#define CoulombCounter_H

#define MICROSCONSTANT 1000000

class CoulombCounter{
	public:
		struct CCValues { //LTC2944 data outs
			double voltage;
			double current;
			double charge;
			double power;
			double energy;//Not a direct output. Calculated using trapezoidal integration of power
			float temperature;
		};

		CCValues ccv;

		CoulombCounter(uint16_t prescaler, float resistor, float vscaler);

		CCValues update();//Should be called as often as possible (every loop) for accurate integration

		double getVoltage();

		double getCurrent();

		double getPower();

		double getCharge();

		float getTemperature();

		double getEnergy();

		uint16_t getPrescalar();

		float getResistor();

		void reset();

	private:
		uint16_t prescalar;//LTC2944 prescaler. Use the macros defined in LTC2944.h
		float resistor;//Resistor value in mOhms
		float vscaler;//If the attachment location of the counter does NOT measure across all cells

		double pwr_p;//previous power measurement
		unsigned long t_c, t_p;//time stamps of previous and current power measurements
};



#endif
