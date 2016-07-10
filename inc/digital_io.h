#pragma once

#include "LPC11xx.h"
#include "pinList.h"

#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3

#ifdef __cplusplus
extern "C" {
#endif

void PIOINT0_IRQHandler(void);
void PIOINT1_IRQHandler(void);
void PIOINT2_IRQHandler(void);
void PIOINT3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

class DigitalPin {
protected:
	uint32_t port;
	uint32_t bit;
	uint32_t value;
	LPC_GPIO_TypeDef* address;

	void setPortAddress(uint32_t port);
	void PortBitConfig(uint32_t pinName);
public:
	DigitalPin(){};
	DigitalPin(uint32_t pinName);
	uint32_t _bit(){return bit;}
};

class DigitalOut : public DigitalPin {
public:
	DigitalOut(){};
	DigitalOut(uint32_t pinName);
	void initPin(uint32_t pinName);
	void write(uint32_t setvalue);
	void operator=(uint32_t setvalue);
	uint32_t read();
};

class DigitalIn : public DigitalPin  {
public:
	DigitalIn(){};
	DigitalIn(uint32_t pinName);
	void initPin(uint32_t pinName);
	uint32_t read();
	operator uint32_t (){return read();}
};

class DigitalInOut : public DigitalPin{
public:
	DigitalInOut(){};
	DigitalInOut(uint32_t pinName);
	void initPin(uint32_t pinName);
	void write(uint32_t setvalue);
	uint32_t read();
	void output();
	void input();
	void operator=(uint32_t setvalue);
	operator uint32_t (){return read();}
};

class InterruptIn : public DigitalIn{
public:
	InterruptIn(uint32_t pinName) : DigitalIn(pinName){
		if(port == PORT0) 	  NVIC_EnableIRQ(EINT0_IRQn);
		else if(port == PORT1) NVIC_EnableIRQ(EINT1_IRQn);
		else if(port == PORT2) NVIC_EnableIRQ(EINT2_IRQn);
		else if(port == PORT3) NVIC_EnableIRQ(EINT3_IRQn);
	}
	void setFuncPtr(void (*fp)(void));
	void enableInterrupt(){address->IE |= (0x1<<bit);}
	void disableInterrupt(){address->IE &= ~(0x1<<bit);}
	uint32_t interruptStatus();
	void clearInterrupt(){address->IC |= (0x1<<bit);};
	void rise(void (*fp)(void));  // interrupt by rising edge
	void fall(void (*fp)(void));  // interrupt by falling edge
};

class BusOut {
private:
	uint32_t nPins;		//the  number of pins
	int32_t value;
	DigitalOut *bus;
public:
	BusOut(uint32_t pin0, uint32_t pin1=NC, uint32_t pin2=NC, uint32_t pin3=NC,
			uint32_t pin4=NC, uint32_t pin5=NC, uint32_t pin6=NC, uint32_t pin7=NC,
			uint32_t pin8=NC, uint32_t pin9=NC, uint32_t pin10=NC, uint32_t pin11=NC,
			uint32_t pin12=NC, uint32_t pin13=NC, uint32_t pin14=NC, uint32_t pin15=NC);
	~BusOut(){delete [] bus;}
	void write(int32_t setvalue);
	void operator=(int32_t setvalue){write(setvalue);}
	int32_t read(){return value;}
	DigitalOut& operator[](uint32_t index){return bus[index];}
};

class BusIn{
private:
	uint32_t nPins;		//the  number of pins
	int32_t value;
	DigitalIn *bus;

public:
	BusIn(uint32_t pin0, uint32_t pin1=NC, uint32_t pin2=NC, uint32_t pin3=NC,
			uint32_t pin4=NC, uint32_t pin5=NC, uint32_t pin6=NC, uint32_t pin7=NC,
			uint32_t pin8=NC, uint32_t pin9=NC, uint32_t pin10=NC, uint32_t pin11=NC,
			uint32_t pin12=NC, uint32_t pin13=NC, uint32_t pin14=NC, uint32_t pin15=NC);
	~BusIn(){delete [] bus;}
	int32_t read();
	operator int32_t (){return read();}
	DigitalIn& operator[](uint32_t index){return bus[index];}
};


class BusInOut{
private:
	uint32_t nPins;		//the  number of pins
	int32_t value;
	DigitalInOut *bus;

public:
	BusInOut(uint32_t pin0, uint32_t pin1=NC, uint32_t pin2=NC, uint32_t pin3=NC,
			uint32_t pin4=NC, uint32_t pin5=NC, uint32_t pin6=NC, uint32_t pin7=NC,
			uint32_t pin8=NC, uint32_t pin9=NC, uint32_t pin10=NC, uint32_t pin11=NC,
			uint32_t pin12=NC, uint32_t pin13=NC, uint32_t pin14=NC, uint32_t pin15=NC);
	~BusInOut(){delete [] bus;}
	void output();
	void input();
	void write(int32_t setvalue);
	void operator=(int32_t setvalue){write(setvalue);}
	int32_t read();
	operator int32_t (){return read();}
	DigitalInOut& operator[](uint32_t index){return bus[index];}
};

