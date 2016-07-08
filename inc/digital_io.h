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

class DigitalOut {
private:
	uint32_t port;
	uint32_t bit;
	uint32_t value;
	LPC_GPIO_TypeDef* address;

	void setPortAddress(uint32_t port);
public:
	DigitalOut(){};
	DigitalOut(uint32_t setport, uint32_t setbit);
	DigitalOut(uint32_t pinName);
	void initializePin(uint32_t pinName);  //same as DigitalOut(uint32_t pinName);
	void write(uint32_t setvalue);
	void operator=(uint32_t setvalue);
	uint32_t read();
	uint32_t _bit(){return bit;}
};

class DigitalIn {
protected:
	uint32_t port;
	uint32_t bit;
	uint32_t value;
	LPC_GPIO_TypeDef* address;

	void setPortAddress(uint32_t port);
public:
	DigitalIn(){};
	DigitalIn(uint32_t setport, uint32_t setbit);
	DigitalIn(uint32_t pinName);
	void initializePin(uint32_t pinName);  //same as DigitalIn(uint32_t pinName);
	uint32_t read();
	uint32_t operator()();
	uint32_t _bit(){return bit;}
};

class DigitalInOut {
private:
	uint32_t port;
	uint32_t bit;
	uint32_t value;
	LPC_GPIO_TypeDef* address;

	void setPortAddress(uint32_t port);
public:
	DigitalInOut(){};
	DigitalInOut(uint32_t setport, uint32_t setbit);
	DigitalInOut(uint32_t pinName);
	void initializePin(uint32_t pinName);  //same as DigitalInOut(uint32_t pinName);
	void write(uint32_t setvalue);
	uint32_t read();
	void output();
	void input();
	void operator=(uint32_t setvalue);
	uint32_t operator()();
};

//割り込み入力
class InterruptIn : public DigitalIn{
public:
	InterruptIn(){};
	InterruptIn(uint32_t setport, uint32_t setbit) : DigitalIn(setport, setbit){
		if(setport == PORT0) 	  NVIC_EnableIRQ(EINT0_IRQn);
		else if(setport == PORT1) NVIC_EnableIRQ(EINT1_IRQn);
		else if(setport == PORT2) NVIC_EnableIRQ(EINT2_IRQn);
		else if(setport == PORT3) NVIC_EnableIRQ(EINT3_IRQn);
	}
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
	void rise(void (*fp)(void));  //立ち上がりエッジで割り込み
	void fall(void (*fp)(void));  //立ち下がりエッジで割り込み
};


class BusOut{
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
	DigitalOut& operator[](uint32_t index);
};

//========== Bus ====================

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
	int32_t operator()();
	DigitalIn& operator[](uint32_t index);
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
	int32_t operator()();
	DigitalInOut& operator[](uint32_t index);
};
