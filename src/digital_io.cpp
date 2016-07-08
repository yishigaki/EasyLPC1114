/*
 * digital_io.cpp
 *
 *  Created on: 2015/08/10
 *      Author: YutaroIshigaki
 */
#include "digital_io.h"

//割り込みハンドラへの関数ポインタ
void (*fptr_isr_gpio0)(void);
void (*fptr_isr_gpio1)(void);
void (*fptr_isr_gpio2)(void);
void (*fptr_isr_gpio3)(void);

//割り込みハンドラ
void PIOINT0_IRQHandler(void)
{
	fptr_isr_gpio0();
}

void PIOINT1_IRQHandler(void)
{
	fptr_isr_gpio1();
}

void PIOINT2_IRQHandler(void)
{
	fptr_isr_gpio2();
}

void PIOINT3_IRQHandler(void)
{
	fptr_isr_gpio3();
}

/********************************************
 *  DigitalOut
 ********************************************/
DigitalOut::DigitalOut(uint32_t setport, uint32_t setbit)
{
	port = setport;
	bit = setbit;

	setPortAddress(port);

	address->DIR |= (0x1<<bit);  //set as output port
}

DigitalOut::DigitalOut(uint32_t pinName)
{
	initializePin(pinName);
}

void DigitalOut::initializePin(uint32_t pinName)
{
	port = pinName / MAXNUMPIN;
	bit = pinName % MAXNUMPIN;

	setPortAddress(port);

	address->DIR |= (0x1<<bit);  //set as output port
}

void DigitalOut::write(uint32_t setvalue)
{
	value = setvalue;
	address->MASKED_ACCESS[(1<<bit)] = (value<<bit);
}

void DigitalOut::operator=(uint32_t setvalue)
{
	value = setvalue;
	address->MASKED_ACCESS[(1<<bit)] = (value<<bit);
}

uint32_t DigitalOut::read()
{
	return value;
}

void DigitalOut::setPortAddress(uint32_t port)
{
	if(port == PORT0){
		address = LPC_GPIO0;
	}else if(port == PORT1){
		address = LPC_GPIO1;
	}else if(port == PORT2){
		address = LPC_GPIO2;
	}else if(port == PORT3){
		address = LPC_GPIO3;
	}else{
		address = LPC_GPIO0;  //exception
	}
}

/********************************************
 *  DigitalIn: PinMode(default): Pull-up
 ********************************************/
DigitalIn::DigitalIn(uint32_t setport, uint32_t setbit)
{
	port = setport;
	bit = setbit;

	setPortAddress(port);
	address->DIR &= ~(0x1<<bit);  //set as input port
}

DigitalIn::DigitalIn(uint32_t pinName)
{
	initializePin(pinName);
}

void DigitalIn::initializePin(uint32_t pinName)
{
	port = pinName / MAXNUMPIN;
	bit = pinName % MAXNUMPIN;

	setPortAddress(port);

	address->DIR &= ~(0x1<<bit);  //set as input port
}

uint32_t DigitalIn::read()
{
	if(!(address->MASKED_ACCESS[(1<<bit)] & (1 << bit))){
		return 0;
	}else{
		return 1;
	}
}

uint32_t DigitalIn::operator()()
{
	if(!(address->MASKED_ACCESS[(1<<bit)] & (1 << bit))){
		return 0;
	}else{
		return 1;
	}
}

void DigitalIn::setPortAddress(uint32_t port)
{
	if(port == PORT0){
		address = LPC_GPIO0;
	}else if(port == PORT1){
		address = LPC_GPIO1;
	}else if(port == PORT2){
		address = LPC_GPIO2;
	}else if(port == PORT3){
		address = LPC_GPIO3;
	}else{
		address = LPC_GPIO0;  //exception
	}
}

/********************************************
 *  DigitalInOut: PinMode(default): Pull-up
 ********************************************/
DigitalInOut::DigitalInOut(uint32_t setport, uint32_t setbit)
{
	port = setport;
	bit = setbit;

	setPortAddress(port);
}

DigitalInOut::DigitalInOut(uint32_t pinName)
{
	initializePin(pinName);
}

void DigitalInOut::initializePin(uint32_t pinName)
{
	port = pinName / MAXNUMPIN;
	bit = pinName % MAXNUMPIN;

	setPortAddress(port);
}

void DigitalInOut::write(uint32_t setvalue)
{
	value = setvalue;
	address->MASKED_ACCESS[(1<<bit)] = (value<<bit);
}

uint32_t DigitalInOut::read()
{
	if(!(address->MASKED_ACCESS[(1<<bit)] & (1 << bit))){
		return 0;
	}else{
		return 1;
	}
}

void DigitalInOut::output()
{
	address->DIR |= (0x1<<bit);  //set as output port
}

void DigitalInOut::input()
{
	address->DIR &= ~(0x1<<bit);  //set as input port
}

void DigitalInOut::operator=(uint32_t setvalue)
{
	value = setvalue;
	address->MASKED_ACCESS[(1<<bit)] = (value<<bit);
}

uint32_t DigitalInOut::operator()()
{
	if(!(address->MASKED_ACCESS[(1<<bit)] & (1 << bit))){
		return 0;
	}else{
		return 1;
	}
}

void DigitalInOut::setPortAddress(uint32_t port)
{
	if(port == PORT0){
		address = LPC_GPIO0;
	}else if(port == PORT1){
		address = LPC_GPIO1;
	}else if(port == PORT2){
		address = LPC_GPIO2;
	}else if(port == PORT3){
		address = LPC_GPIO3;
	}else{
		address = LPC_GPIO0;  //exception
	}
}

/********************************************
 *  InterruptIn
 ********************************************/
/*InterruptIn(uint32_t setport, uint32_t setbit) : DigitalIn(setport, setbit)
{
}
*/

void InterruptIn::setFuncPtr(void (*fp)(void))
{
	if(port == PORT0){
		fptr_isr_gpio0 = fp;
	}else if(port == PORT1){
		fptr_isr_gpio1 = fp;
	}else if(port == PORT2){
		fptr_isr_gpio2 = fp;
	}else if(port == PORT3){
		fptr_isr_gpio3 = fp;
	}else{
		fptr_isr_gpio0 = fp;  //exception
	}
}

uint32_t InterruptIn::interruptStatus()
{
	uint32_t regVal = 0;
	if(address->MIS & (0x1<<bit)) regVal = 1;

	return regVal;
}

void InterruptIn::rise(void (*fp)(void))
{
	setFuncPtr(fp);

	address->IS &= ~(0x1<<bit);  //configured as edge sensitive
	address->IBE &= ~(0x1<<bit); //configured as single edge
	address->IEV |= (0x1<<bit);  //configured as rising edge

	enableInterrupt();
}

void InterruptIn::fall(void (*fp)(void))
{
	setFuncPtr(fp);

	address->IS &= ~(0x1<<bit);  //configured as edge sensitive
	address->IBE &= ~(0x1<<bit); //configured as single edge
	address->IEV &= ~(0x1<<bit); //configured as falling edge

	enableInterrupt();
}

// ============= Bus =========================

/********************************************
 *  BusOut
 ********************************************/
BusOut::BusOut(uint32_t pin0, uint32_t pin1, uint32_t pin2, uint32_t pin3,
			uint32_t pin4, uint32_t pin5, uint32_t pin6, uint32_t pin7,
			uint32_t pin8, uint32_t pin9, uint32_t pin10, uint32_t pin11,
			uint32_t pin12, uint32_t pin13, uint32_t pin14, uint32_t pin15)
{
	nPins = 0;
	uint32_t pinlist[MAXNUMPIN] = {pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7,
									pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15};
	for(int i=0; i<MAXNUMPIN; i++){
		if(pinlist[i] != NC) nPins++;
	}

	bus = new DigitalOut [nPins];
	for(uint32_t i=0; i<nPins; i++){
		bus[i].initializePin(pinlist[i]);
	}
}

void BusOut::write(int32_t setvalue)
{
	value = setvalue;

	uint16_t mask = 0x0001;
	for(uint32_t i=0; i<nPins; i++){
		if((setvalue & mask)==0){
			bus[i] = 0;
		}else{
			bus[i] = 1;
		}
		mask = mask << 1;
	}
}

DigitalOut& BusOut::operator[](uint32_t index)
{
	return bus[index];
}


/********************************************
 *  BusIn
 ********************************************/
BusIn::BusIn(uint32_t pin0, uint32_t pin1, uint32_t pin2, uint32_t pin3,
			uint32_t pin4, uint32_t pin5, uint32_t pin6, uint32_t pin7,
			uint32_t pin8, uint32_t pin9, uint32_t pin10, uint32_t pin11,
			uint32_t pin12, uint32_t pin13, uint32_t pin14, uint32_t pin15)
{
	nPins = 0;
	uint32_t pinlist[MAXNUMPIN] = {pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7,
									pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15};
	for(int i=0; i<MAXNUMPIN; i++){
		if(pinlist[i] != NC) nPins++;
	}

	bus = new DigitalIn [nPins];
	for(uint32_t i=0; i<nPins; i++){
		bus[i].initializePin(pinlist[i]);
	}
}

int32_t BusIn::read()
{
	int32_t tempVal = 0x0000;

	uint16_t mask = 0x0001;
	for(uint32_t i=0; i<nPins; i++){
		if(bus[i].read()!=0){
			tempVal |= mask;
		}
		mask = mask << 1;
	}

	value = tempVal;
	return tempVal;
}

int32_t BusIn::operator ()()
{
	int32_t val = read();
	return val;
}

DigitalIn& BusIn::operator[](uint32_t index)
{
	return bus[index];
}


/********************************************
 *  BusInOut
 ********************************************/
BusInOut::BusInOut(uint32_t pin0, uint32_t pin1, uint32_t pin2, uint32_t pin3,
			uint32_t pin4, uint32_t pin5, uint32_t pin6, uint32_t pin7,
			uint32_t pin8, uint32_t pin9, uint32_t pin10, uint32_t pin11,
			uint32_t pin12, uint32_t pin13, uint32_t pin14, uint32_t pin15)
{
	nPins = 0;
	uint32_t pinlist[MAXNUMPIN] = {pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7,
									pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15};
	for(int i=0; i<MAXNUMPIN; i++){
		if(pinlist[i] != NC) nPins++;
	}

	bus = new DigitalInOut [nPins];
	for(uint32_t i=0; i<nPins; i++){
		bus[i].initializePin(pinlist[i]);
	}
}

void BusInOut::output()
{
	for(uint32_t i=0; i<nPins; i++){
		bus[i].output();
	}
}

void BusInOut::input()
{
	for(uint32_t i=0; i<nPins; i++){
		bus[i].input();
	}
}

void BusInOut::write(int32_t setvalue)
{
	value = setvalue;

	uint16_t mask = 0x0001;
	for(uint32_t i=0; i<nPins; i++){
		if((setvalue & mask)==0){
			bus[i] = 0;
		}else{
			bus[i] = 1;
		}
		mask = mask << 1;
	}
}

int32_t BusInOut::read()
{
	int32_t tempVal = 0x0000;

	uint16_t mask = 0x0001;
	for(uint32_t i=0; i<nPins; i++){
		if(bus[i].read()!=0){
			tempVal |= mask;
		}
		mask = mask << 1;
	}

	value = tempVal;
	return tempVal;
}

int32_t BusInOut::operator ()()
{
	int32_t val = read();
	return val;
}


DigitalInOut& BusInOut::operator[](uint32_t index)
{
	return bus[index];
}
