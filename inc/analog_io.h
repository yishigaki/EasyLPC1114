#pragma once

#include "LPC11xx.h"

#define AD0 0
#define AD1 1
#define AD2 2
#define AD3 3
#define AD4 4
#define AD5 5

#define ADC_MAX_VAL 1023  //10-bit ADC -> 1023

class AnalogIn {
private:
	uint32_t adc_number;
	uint32_t address;
	float refVoltage;
	uint32_t clkdiv;
	uint32_t control;

public:
	AnalogIn(uint32_t selectADC_num);
	void setRefVoltage(float voltage){refVoltage = voltage;}
	uint32_t read();
	float read_voltage();
};
