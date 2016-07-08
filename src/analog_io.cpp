#include "analog_io.h"

AnalogIn::AnalogIn(uint32_t selectADC_num)
{
	adc_number = selectADC_num;

	//set pin function as ADC
	switch(adc_number){
	case AD0:
		LPC_IOCON->R_PIO0_11 = 0x02; break;
	case AD1:
		LPC_IOCON->R_PIO1_0 = 0x02; break;
	case AD2:
		LPC_IOCON->R_PIO1_1 = 0x02; break;
	case AD3:
		LPC_IOCON->R_PIO1_2 = 0x02; break;
	case AD4:
		LPC_IOCON->SWDIO_PIO1_3 = 0x02; break;
	case AD5:
		LPC_IOCON->PIO1_4 = 0x01; break;
	}

	LPC_SYSCON->PDRUNCFG      &= ~(1 << 4);  //Supply power for ADC
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);  //system clock 供給

	refVoltage = 3.3;  //default
	uint32_t clkdiv = (SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/4500000; //clock must be less than 4.5MHz
	control = (clkdiv << 8) | (0x1 << adc_number);
}

uint32_t AnalogIn::read()
{
	uint32_t adc_dataReg;

	uint32_t command = control | 0x1000000;  //start A/D conversion
	LPC_ADC->CR = command;
	//wait until conversion has done
	do{
		adc_dataReg = LPC_ADC->DR[adc_number];
	}while((adc_dataReg & 0x80000000) == 0);

	uint32_t result = (adc_dataReg >> 6) & 0x3FF; //10-bit mask

	return result;
}

float AnalogIn::read_voltage()
{
	float result;
	uint32_t intdata = read();
	result = (float)intdata * refVoltage / (float)ADC_MAX_VAL ;

	return result;
}
